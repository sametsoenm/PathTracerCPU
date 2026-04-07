#pragma once

#include "material.h"
#include "core/intersection.h"

class LambertDiffuseMaterial : public Material {

public:
	LambertDiffuseMaterial() :
		_albedoTex(glm::vec3(1.0f)) {}

	explicit LambertDiffuseMaterial(const glm::vec3& __albedo) :
		_albedoTex(__albedo) {}

	explicit LambertDiffuseMaterial(
		const Texture& __albedoTex) :
		_albedoTex(__albedoTex) {}

	explicit LambertDiffuseMaterial(
		const Texture& __albedoTex, 
		const Texture& __normalTex) :
		_albedoTex(__albedoTex), _normalTex(__normalTex), _hasNormalMap(true) {}

	BSDFSample sample(
		const glm::vec3& wo, const Intersection& rec, const glm::vec2& u) const override;

	float pdf(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const override;

	glm::vec3 eval_bsdf_cos(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const override;

	glm::vec3 color(const Intersection& rec) const override {
		return _albedoTex.sampleNearest(rec.uv.x, rec.uv.y);
	}

private:
	Texture _albedoTex;
	Texture _normalTex;
	bool _hasNormalMap = false;
};