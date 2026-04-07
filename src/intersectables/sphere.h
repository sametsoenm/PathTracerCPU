#pragma once
#include <glm/vec3.hpp>
#include "intersectable.h"

#include "material/lambert_diffuse.h"

#include <memory>


class Sphere : public Intersectable {
public:
	Sphere() :
		_radius(1.0f), _origin(0.0f), 
		_material(std::make_shared<LambertDiffuseMaterial>(glm::vec3(1.0f))) {}

	Sphere(float __radius, const glm::vec3& __origin, std::shared_ptr<Material> __material) :
		_radius(__radius), _origin(__origin), _material(std::move(__material)) {}

	bool intersect(
		const Ray& ray, float t_min, float t_max, Intersection& record) const override;
	bool bbox(AABB& box) const override;

private:
	float _radius;
	glm::vec3 _origin;
	std::shared_ptr<Material> _material;
};