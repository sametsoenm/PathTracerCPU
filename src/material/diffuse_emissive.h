#pragma once

#include "material.h"
#include "core/intersection.h"

class DiffuseEmissiveMaterial : public Material {

public:
	explicit DiffuseEmissiveMaterial() :
		_emission(glm::vec3(10.0f)) {
	}

	explicit DiffuseEmissiveMaterial(const glm::vec3& __emission) :
		_emission(__emission) {
	}

	BSDFSample sample(
		const glm::vec3& wo, const Intersection& rec, const glm::vec2& u) const override {
		return BSDFSample{};
	}

	float pdf(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const override {
		return 0.0f;
	}

	glm::vec3 eval_bsdf_cos(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const override {
		return glm::vec3(0.0f);
	}

	glm::vec3 emission() const override {
		return _emission;
	}

	glm::vec3 color(const Intersection& rec) const override {
		return _emission;
	}

	bool is_emissive() const { return true; }


private:
	glm::vec3 _emission;
};