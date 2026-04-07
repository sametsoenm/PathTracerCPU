#pragma once

#include <glm/vec3.hpp>
#include "core/ray.h"

#include <utility>

struct AABB {
	glm::vec3 min;
	glm::vec3 max;

	AABB() = default;
	AABB(const glm::vec3& __min, const glm::vec3 __max) :
		min(__min), max(__max) {}
	
	bool intersect(const Ray& ray, float t_min, float t_max) const {
		for (int axis = 0; axis < 3; axis++) {
			const float invD = 1.0f / ray.dir[axis];
			float t0 = (min[axis] - ray.o[axis]) * invD;
			float t1 = (max[axis] - ray.o[axis]) * invD;

			if (invD < 0.0f)
				std::swap(t0, t1);

			t_min = t0 > t_min ? t0 : t_min;
			t_max = t1 < t_max ? t1 : t_max;

			if (t_max <= t_min) return 
				false;
		}
		return true;
	}
};

namespace aabb {

	inline AABB surrounding_box(const AABB& a, const AABB& b) {
		const glm::vec3 min(
			std::min(a.min.x, b.min.x),
			std::min(a.min.y, b.min.y),
			std::min(a.min.z, b.min.z)
		);
		const glm::vec3 max(
			std::max(a.max.x, b.max.x),
			std::max(a.max.y, b.max.y),
			std::max(a.max.z, b.max.z)
		);
		return AABB(min, max);
	}

	inline int longest_axis(const AABB& box) {
		const glm::vec3 diff = box.max - box.min;
		if (diff.x > diff.y && diff.x > diff.z)
			return 0;
		if (diff.y > diff.x)
			return 1;
		return 2;
	}

	inline float box_center_on_axis(const AABB& box, int axis) {
		return 0.5f * (box.min[axis] + box.max[axis]);
	}

}