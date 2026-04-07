#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include "material/material.h"

class Light;

struct Intersection {
	float t = FLT_MAX;
	glm::vec3 pos{ 0.0f };
	glm::vec2 uv{ 0.0f };

	glm::vec3 normal{ 0.0f }; // geometric normal
	glm::vec3 shading_normal{ 0.0f };
	glm::vec3 tangent{ 0.0f };

	bool front_face = true;

	std::shared_ptr<Material> material = nullptr;
	const Light* light = nullptr;

	void set_face_normal(const glm::vec3& ray_dir, const glm::vec3& outside_normal) {
		front_face = glm::dot(ray_dir, outside_normal) < 0.0f;
		normal = (front_face || (material && material->is_emissive()))
			? outside_normal
			: -outside_normal;
	}
};