#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include "util/constants.h"
#include "util/texture.h"

#include <complex.h>

struct Intersection;

struct BSDFSample {
	glm::vec3 wi{ 0.0f };
	glm::vec3 bsdf_cos{ 0.0f };
	float pdf = 0.0f;
	glm::vec3 contrib{ 0.0f }; // returns (bsdf/pdf) * cos
	bool is_delta = false;
	float sign = 1.0; // 1 if reflected, -1 if transmitted
};

class Material {
public:

	virtual ~Material() = default;

	virtual BSDFSample sample(
		const glm::vec3& wo, const Intersection& rec, const glm::vec2& u) const = 0;

	virtual float pdf(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const = 0;

	virtual glm::vec3 eval_bsdf_cos(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const = 0;

	virtual glm::vec3 emission() const {
		return glm::vec3(0.0f);
	}

	virtual glm::vec3 color(const Intersection& rec) const = 0;

	virtual bool is_emissive() const { return false; }

};

