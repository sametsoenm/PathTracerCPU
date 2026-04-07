#pragma once

#include "material.h"
#include "core/intersection.h"

class SpecularMicrofacetMaterial : public Material {

public:
	SpecularMicrofacetMaterial() :
		_f0Tex(material::metal::F0_GOLD), _alphaTex(glm::vec3(0.2f)),
		_eta(material::metal::ETA_GOLD_RGB),
		_kappa(material::metal::KAPPA_GOLD_RGB) {
	}

	SpecularMicrofacetMaterial(const glm::vec3& __f0, const float __alpha) :
		_f0Tex(__f0), _alphaTex(glm::vec3(__alpha)),
		_eta(glm::vec3(0.0f)), _kappa(glm::vec3(0.0f)) {
	}

	SpecularMicrofacetMaterial(Texture __f0, const float __alpha) :
		_f0Tex(__f0), _alphaTex(glm::vec3(__alpha)),
		_eta(glm::vec3(0.0f)), _kappa(glm::vec3(0.0f)) {
	}

	SpecularMicrofacetMaterial(const glm::vec3& __f0, Texture __alpha) :
		_f0Tex(__f0), _alphaTex(__alpha),
		_eta(glm::vec3(0.0f)), _kappa(glm::vec3(0.0f)) {
	}

	SpecularMicrofacetMaterial(Texture __f0, Texture __alpha) :
		_f0Tex(__f0), _alphaTex(__alpha),
		_eta(glm::vec3(0.0f)), _kappa(glm::vec3(0.0f)) {
	}

	SpecularMicrofacetMaterial(Texture __f0, Texture __alpha, Texture __normalTex) :
		_f0Tex(__f0), _alphaTex(__alpha), _normalTex(__normalTex), _hasNormalMap(true),
		_eta(glm::vec3(0.0f)), _kappa(glm::vec3(0.0f)) {
	}

	SpecularMicrofacetMaterial(
		const glm::vec3& __eta, const glm::vec3& __kappa, const float __alpha) :
		_f0Tex(glm::vec3(0.0f)), _alphaTex(glm::vec3(__alpha)),
		_eta(__eta), _kappa(__kappa) {
	}

	BSDFSample sample(
		const glm::vec3& wo, const Intersection& rec, const glm::vec2& u) const override;

	float pdf(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const override;

	// evaluates bsdf * cos(N,wi)
	glm::vec3 eval_bsdf_cos(
		const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const override;

	glm::vec3 color(const Intersection& rec) const override {
		return _f0Tex.sample(rec.uv.x, rec.uv.y);
	}

private:
	Texture _alphaTex;
	Texture _f0Tex;
	glm::vec3 _eta;
	glm::vec3 _kappa;

	Texture _normalTex;
	bool _hasNormalMap = false;
};