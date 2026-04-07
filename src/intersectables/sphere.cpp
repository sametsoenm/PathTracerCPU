#include "sphere.h"
#include <glm/geometric.hpp>
#include "util/texture.h"

bool Sphere::intersect(const Ray& ray, float t_min, float t_max, Intersection& record) const {
	glm::vec3 oc = _origin - ray.o;
	float a = glm::dot(ray.dir, ray.dir);
	float h = glm::dot(ray.dir, oc);
	float c = glm::dot(oc, oc) - _radius * _radius;
	float discriminant = h * h - a * c;

	if (discriminant < 0.0f) 
		return false;

	float root = glm::sqrt(discriminant);
	float res = (h - root) / a;
	if (res <= t_min || res > t_max) {
		res = (h + root) / a;
		if (res <= t_min || res > t_max)
			return false;
	}

	record.t = res;
	record.pos = ray.at(res);
	const glm::vec3 out_normal = (record.pos - _origin) / _radius;
	record.uv = texture::directionToUV(out_normal);
	record.material = _material;
	record.set_face_normal(ray.dir, out_normal);
	record.shading_normal = record.normal;
	record.tangent = glm::vec3(-out_normal.z, 0.0f, out_normal.x);

	return true;
}

bool Sphere::bbox(AABB& box) const {
	box.min = _origin - glm::vec3(_radius);
	box.max = _origin + glm::vec3(_radius);
	return true;
}