#pragma once

#include "material.h"

class MirrorMaterial : public Material {

public:
	MirrorMaterial() :
		_reflectance(glm::vec3(1.0f)) {}

	explicit MirrorMaterial(const glm::vec3& __reflectance) :
		_reflectance(__reflectance) {
	}

	BSDFSample sample(
		const glm::vec3& wo, const Intersection& rec, const glm::vec2& u) const override;

	float pdf(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const override;

	glm::vec3 eval_bsdf_cos(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const override;

	glm::vec3 color(const Intersection& rec) const override {
		return _reflectance;
	}

private:
	glm::vec3 _reflectance;
};