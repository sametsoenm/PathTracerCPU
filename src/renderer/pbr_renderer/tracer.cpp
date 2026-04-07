#include "../renderer.h"
#include "util/sampling.h"

#include <iostream>

glm::vec3 PBRRenderer::traceRay(const Ray& start_ray, RNG& rng) const {

	PathTraceData pt_data;
	pt_data.ray.o = start_ray.o;
	pt_data.ray.dir = start_ray.dir;

	glm::vec3 color(0.0f);
	glm::vec3 attenuation(1.0f);

	for (size_t depth = 0; depth < settings::MAX_RAY_DEPTH; depth++) {
		pt_data.depth = depth;
		pt_data.contribution = glm::vec3(0.0f);
		pt_data.exit = false;

		Intersection rec;
		const bool hit_surface = _scene->hasWorld() && _scene->world().intersect(
			pt_data.ray, settings::T_MIN, settings::T_MAX, rec);
		const float t_surface = hit_surface ? rec.t : settings::T_MAX;
		const float t_medium = _scene->fogEnabled()
			? -glm::log(rng.next_float()) / _scene->fogDensity()
			: settings::T_MAX;

		if (t_medium < t_surface) {
			pt_data.t_sample = t_medium;
			volumeEvent(rng, pt_data);
		}
		else if (hit_surface) {
			closestHit(rec, pt_data, rng);
		}
		else {
			miss(rec, pt_data);
		}

		color += pt_data.contribution * attenuation;

		// if lightsource was hit
		if (pt_data.exit)
			break;

		attenuation *= pt_data.attenuation_step;

		// Russian Roulette after 3 bounces
		if (depth >= 3) {
			// calc luminance
			float p = 0.2126f * attenuation.r
				+ 0.7152f * attenuation.g
				+ 0.0722f * attenuation.b;
			p = glm::clamp(p, 0.05f, 0.95f);

			if (rng.next_float() > p)
				break;

			attenuation /= p;
		}
	}

	return color;
}

