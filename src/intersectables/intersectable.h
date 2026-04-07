#pragma once

#include <glm/vec3.hpp>

#include "core/ray.h"
#include "core/intersection.h"
#include "util/aabb.h"

class Intersectable {

public:
	virtual ~Intersectable() = default;

	virtual bool intersect(
		const Ray& ray, float t_min, float t_max, Intersection& record) const = 0;
	
	virtual bool bbox(AABB& box) const = 0; // bool if bounding box exists
};
