#include "mirror.h"

#include <glm/geometric.hpp>
#include "core/intersection.h"

BSDFSample MirrorMaterial::sample(
	const glm::vec3& wo, const Intersection& rec, const glm::vec2& u) const {

	BSDFSample s{};

	const glm::vec3 wi = glm::reflect(-wo, rec.normal);
	s.wi = wi;
	s.pdf = 1.0f;
	s.contrib = _reflectance;
	s.is_delta = true;

	return s;
}

float MirrorMaterial::pdf(
	const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const {

	return 0.0f;
}

glm::vec3 MirrorMaterial::eval_bsdf_cos(
	const glm::vec3& wo, const glm::vec3& wi, const Intersection& rec) const {

	// const glm::vec3 wr = glm::reflect(-wo, rec.normal);
	// const float cos_theta = glm::max(0.0f, glm::dot(wi, rec.normal));
	// return (wr == wo) ? glm::vec3(1.0f) / cos_theta : glm::vec3(0.0f);

	(void)wo; (void)wi; (void)rec;
	return glm::vec3(0.0f);
}