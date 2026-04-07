#include "dielectric.h"
#include "material_funcs.h"

BSDFSample DielectricMaterial::sample(
	const glm::vec3& wo, const Intersection& rec, const glm::vec2& u) const {

	BSDFSample s{};
	s.is_delta = true;

	float etaI = 1.0f; // air IOR
	float etaJ = _eta;
	if (!rec.front_face) {
		etaI = _eta;
		etaJ = 1.0f;
	}

	const float cosr = glm::max(0.0f, glm::dot(wo, rec.shading_normal));
	const float eta = etaI / etaJ;
	const float sint2 = eta * eta * (1.0f - cosr * cosr);

	// check for total internal reflection
	if (sint2 > 1.0f) {
		s.wi = glm::reflect(-wo, rec.shading_normal);
		s.pdf = 1.0f; // maybe adjust for NEE
		s.contrib = glm::vec3(1.0f); // pdf and bsdf not math accurate
		return s;
	}

	const float cost = glm::sqrt(1.0f - sint2);
	const float F = material::fresnel_dielectric(etaI, etaJ, cosr, cost);

	// reflection
	if (u.x < F) {
		s.wi = glm::reflect(-wo, rec.shading_normal);
		s.pdf = 1.0f;
		s.contrib = glm::vec3(1.0f);
		return s;
	}

	// transmission
	s.wi = glm::normalize(material::refract(-wo, rec.shading_normal, etaI, etaJ));
	s.pdf = 1.0f;
	const float weight = etaJ * etaJ / etaI / etaI;
	s.contrib = glm::vec3(weight);
	s.sign = -1.0f;

	return s;
}

float DielectricMaterial::pdf(
	const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const {

	return 0.0f;
}

glm::vec3 DielectricMaterial::eval_bsdf_cos(
	const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const {

	(void)wo; (void)wi; (void)rec;
	return glm::vec3(0.0f);
}