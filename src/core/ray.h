#pragma once

#include <glm/vec3.hpp>

struct Ray {
	Ray() :
		o(0.0f), dir(0.0f, 0.0f, -1.0f) {}

	Ray(const glm::vec3& origin, const glm::vec3& direction) :
		o(origin), dir(direction) {}

	glm::vec3 at(float t) const {
		return o + t * dir;
	}

	glm::vec3 o;
	glm::vec3 dir;
};