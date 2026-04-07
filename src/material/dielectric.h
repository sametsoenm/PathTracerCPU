#pragma once

#include "material.h"
#include "core/intersection.h"

class DielectricMaterial : public Material {

public:
	DielectricMaterial() :
		_eta(1.55f) {
	}

	explicit DielectricMaterial(const float __eta) :
		_eta(__eta) {
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

};