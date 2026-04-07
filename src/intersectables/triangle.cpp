#include "triangle.h"

#include "util/constants.h"

// Möller-Trumbone algorithm
bool Triangle::intersect(
	const Ray& ray, float t_min, float t_max, Intersection& record) const {

    const glm::vec3 pvec = glm::cross(ray.dir, _edge2);
    const float det = glm::dot(_edge1, pvec);

    if (std::abs(det) < math::EPSILON)
        return false;

    const float inv_det = 1.0f / det;
    const glm::vec3 tvec = ray.o - _v0;

    const float u = glm::dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f)
        return false;

    const glm::vec3 qvec = glm::cross(tvec, _edge1);
    const float v = glm::dot(ray.dir, qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f)
        return false;

    const float t = glm::dot(_edge2, qvec) * inv_det;
    if (t < t_min || t > t_max)
        return false;

    const float w = (1.0f - u - v);

    record.t = t;
    record.pos = ray.at(t);
    record.uv = w * _uv0 + u * _uv1 + v * _uv2;
    record.material = _material;

    if (_material->is_emissive())
        record.light = this;

    if (!_hasVertexNormals) {
        record.set_face_normal(ray.dir, _normalFlat);
        record.shading_normal = record.normal;
    }
    else {
        const glm::vec3 shading_normal = glm::normalize(w * _n0 + u * _n1 + v * _n2);

        record.set_face_normal(ray.dir, _normalFlat);
        record.shading_normal = record.front_face
            ? shading_normal
            : -shading_normal;
    }

    record.tangent = _tangent;

    return true;
}

bool Triangle::bbox(AABB& box) const {
    // maybe epsilon
    box.min = glm::vec3(
        glm::min(_v0.x, glm::min(_v1.x, _v2.x)) - math::EPSILON,
        glm::min(_v0.y, glm::min(_v1.y, _v2.y)) - math::EPSILON,
        glm::min(_v0.z, glm::min(_v1.z, _v2.z)) - math::EPSILON
    );
    box.max = glm::vec3(
        glm::max(_v0.x, glm::max(_v1.x, _v2.x)) + math::EPSILON,
        glm::max(_v0.y, glm::max(_v1.y, _v2.y)) + math::EPSILON,
        glm::max(_v0.z, glm::max(_v1.z, _v2.z)) + math::EPSILON
    );
    return true;
}

LightSample Triangle::sample(const glm::vec3& pos, const glm::vec3& u) const {
    LightSample s{};

    float su = std::sqrt(u.x); 

    float b0 = 1.0f - su;
    float b1 = u.y * su;
    float b2 = 1.0f - b0 - b1;

    s.pos = b0 * _v0 + b1 * _v1 + b2 * _v2;
    s.emission = emission();
    s.normal = normal();
    s.pdf = pdf(pos, s.pos, s.normal);

    return s;
}

float Triangle::pdf(
    const glm::vec3& hit_pos,
    const glm::vec3& light_pos,
    const glm::vec3& light_normal) const {
    // conversion to solid angle space
    glm::vec3 wi = light_pos - hit_pos;
    const float d2 = glm::dot(wi, wi);
    wi = glm::normalize(wi);

    float cosThetaJ = glm::max(1e-8f, glm::dot(light_normal, -wi));
    return 1.0f / area() * d2 / cosThetaJ;
}

float Triangle::pdf_Li(
    const glm::vec3& hit_pos,
    const glm::vec3& light_pos,
    const glm::vec3& light_normal) const {
    if (!contains(hit_pos))
        return 0.0f;

    return pdf(hit_pos, light_pos, light_normal);
}

bool Triangle::contains(const glm::vec3& p) const {
    float planeDist = glm::dot(p - _v0, _normalFlat);
    if (glm::abs(planeDist) > 1e-4f)
        return false;

    glm::vec3 v0p = p - _v0;

    float d00 = glm::dot(_edge1, _edge1);
    float d01 = glm::dot(_edge1, _edge2);
    float d11 = glm::dot(_edge2, _edge2);
    float d20 = glm::dot(v0p, _edge1);
    float d21 = glm::dot(v0p, _edge2);

    float denom = d00 * d11 - d01 * d01;
    if (glm::abs(denom) < 1e-8f)
        return false;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    const float eps = 1e-4f;
    return u >= -eps && v >= -eps && w >= -eps;
}

void Triangle::updateGeometry() {
    _edge1 = _v1 - _v0;
    _edge2 = _v2 - _v0;
    _normalFlat = glm::normalize(glm::cross(_edge1, _edge2));

    // calc tangent
    const glm::vec2 duv1 = _uv1 - _uv0;
    const glm::vec2 duv2 = _uv2 - _uv0;

    float invDet = 1.0f / duv1.x * duv2.y - duv1.y * duv2.x;
    _tangent = (_edge1 * duv2.y - _edge2 * duv1.y) * invDet;
    _tangent = glm::normalize(_tangent);

}