#include "light.h"

LightSample AreaLight::sample(const glm::vec3& pos, const glm::vec3& u) const {
	LightSample s{};
	s.pos = _origin + u.x * _u + u.y * _v;
	s.emission = emission();
	s.normal = normal();
	s.pdf = pdf(pos, s.pos, s.normal);
	s.is_hittable = false;
	return s;
}

float AreaLight::pdf(
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

bool AreaLight::contains(const glm::vec3& p) const {
	glm::vec3 n = normal();
	float plane_dist = glm::abs(glm::dot(p - _origin, n));
	if (plane_dist > 1e-4f)
		return false;

	glm::vec3 d = p - _origin;

	float uu = glm::dot(_u, _u);
	float uv = glm::dot(_u, _v);
	float vv = glm::dot(_v, _v);
	float du = glm::dot(d, _u);
	float dv = glm::dot(d, _v);

	float det = uu * vv - uv * uv;
	if (glm::abs(det) < 1e-8f)
		return false;

	float a = (du * vv - dv * uv) / det;
	float b = (dv * uu - du * uv) / det;

	return a >= 0.0f && a <= 1.0f && b >= 0.0f && b <= 1.0f;
}

float AreaLight::pdf_Li(
	const glm::vec3& hit_pos,
	const glm::vec3& light_pos,
	const glm::vec3& light_normal) const {
	if (!contains(light_pos))
		return 0.0f;

	return pdf(hit_pos, light_pos, light_normal);
}

LightSample LightList::sample(const glm::vec3& pos, const glm::vec3& u) const {
	LightSample s{};

	if (_lights.empty())
		return s;

	// randomly choose one light source
	const size_t idx = static_cast<size_t>( u.x * _lights.size());
	const auto light = _lights[idx];

	// sample point on light source
	LightSample temp{};
	temp = light->sample(pos, glm::vec3(u.y, u.z, 0.0f));

	s.emission = temp.emission;
	s.normal = temp.normal;
	s.pos = temp.pos;
	s.pdf = temp.pdf / _lights.size();
	s.is_hittable = temp.is_hittable;

	return s;
}

float LightList::pdf_Li(
	const glm::vec3& hit_pos,
	const glm::vec3& light_pos,
	const glm::vec3& light_normal) const const {
	for (auto l : _lights) {
		const float pdf = l->pdf_Li(hit_pos, light_pos, light_normal);
		if (pdf > 0.0f)
			return pdf / static_cast<float>(_lights.size());
	}
	return 0.0f;
}

LightSample EnvironmentLight::sample(const glm::vec3& pos, const glm::vec3& u) const {
	return LightSample{};
}

float EnvironmentLight::pdf(
	const glm::vec3& hit_pos,
	const glm::vec3& light_pos,
	const glm::vec3& light_normal) const {
	return 0.0f;
}

float EnvironmentLight::pdf_Li(
	const glm::vec3& hit_pos,
	const glm::vec3& light_pos,
	const glm::vec3& light_normal) const {
	return 0.0f;
}

glm::vec3 EnvironmentLight::emission() const {
	return glm::vec3(0.0f);
}

bool posColor(const glm::vec3& c) {
	return (c.r > 0.0f) && (c.g > 0.0f) && (c.b > 0.0f);
}

glm::vec3 EnvironmentLight::eval(const glm::vec3& dir) const {
	if (!_texture.valid())
		return glm::vec3(0.0f);

	const glm::vec2 coords = texture::directionToUV(dir);
	return _texture.sample(coords.x, coords.y);
}