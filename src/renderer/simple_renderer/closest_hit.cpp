#include "../renderer.h"

#include <glm/exponential.hpp>
#include <util/sampling.h>

#include <iostream>

void SimpleRenderer::closestHit(const Intersection& rec, PathTraceData& pt_data, RNG& rng) const {

	if (rec.material->is_emissive()) {
		pt_data.contribution = rec.material->emission();
		return;
	}

	const glm::vec3 N = rec.shading_normal;
	const glm::vec3 wo = -glm::normalize(pt_data.ray.dir);
	const glm::vec3 wl = glm::normalize(glm::vec3(1.0f));

	const glm::vec3 bsdf = rec.material->eval_bsdf_cos(wo, wl, rec);
	const glm::vec3 color = rec.material->color(rec);
	glm::vec3 radiance = 0.5f * (bsdf + color) + 0.2f * color;
	pt_data.contribution = radiance;
	

}

