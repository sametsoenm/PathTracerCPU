#include "lambert_diffuse.h"

#include "util/sampling.h"
#include "util/onb.h"
#include "material_funcs.h"

#include <glm/geometric.hpp>
#include <glm/common.hpp>

BSDFSample LambertDiffuseMaterial::sample(
	const glm::vec3& wo, const Intersection& rec, const glm::vec2& u) const {

	BSDFSample s{};
	
	// sample locally
	glm::vec3 wi = sampling::sample_cosine_hemisphere(u);
	Onb onb(rec.normal);
	// transform to world space
	onb.to_world_inplace(wi);

	const glm::vec3 N = _hasNormalMap
		? material::mapNormal(rec, _normalTex)
		: rec.shading_normal;

	if (glm::dot(wi, N) <= 0.0f) {
		s.wi = wi;
		s.pdf = 0.0f;
		s.contrib = glm::vec3(0.0f);
		return s;
	}

	const float V = glm::dot(rec.normal, wi) > 0.0f
		? 1.0f : 0.0f;

	const glm::vec3 albedo = _albedoTex.sample(rec.uv.x, rec.uv.y);

	s.contrib = V * albedo;
	s.pdf = pdf(wo, wi, rec);
	s.wi = wi;

	return s;
}

float LambertDiffuseMaterial::pdf(
	const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const {
	(void)wo;

	const float cos_theta = glm::max(0.0f, glm::dot(wi, rec.normal));
	return sampling::pdf_cosine_hemisphere(cos_theta);
}

glm::vec3 LambertDiffuseMaterial::eval_bsdf_cos(
	const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const {
	(void)wo;

	const glm::vec3 N = _hasNormalMap
		? material::mapNormal(rec, _normalTex)
		: rec.shading_normal;
	if (glm::dot(wi, N) <= 0.0f)
		return glm::vec3(0.0f);

	const float cosTheta = glm::dot(rec.normal, wi) > 0.0f 
		? glm::max(0.0f, glm::dot(N, wi)) : 0.0f;

	const glm::vec3 albedo = _albedoTex.sample(rec.uv.x, rec.uv.y);
	return (albedo / math::PI) * cosTheta;
}
