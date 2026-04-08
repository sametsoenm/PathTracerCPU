#pragma once

#include "material.h"
#include "core/intersection.h"

class RoughDielectricMaterial : public Material {

public:
	RoughDielectricMaterial() :
		_eta(1.55f), _alphaTex(glm::vec3(0.2f)) {
	}

	RoughDielectricMaterial(const float __eta, const Texture& __alpha) :
		_eta(__eta), _alphaTex(__alpha) {
	}

	BSDFSample sample(
		const glm::vec3& wo, const Intersection& rec, const glm::vec2& u) const override;

	float pdf(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const override;

	glm::vec3 eval_bsdf_cos(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const override;

	glm::vec3 color(const Intersection& rec) const override {
		return glm::vec3(0.9f);
	}

private:
	float _eta;
	Texture _alphaTex;
};