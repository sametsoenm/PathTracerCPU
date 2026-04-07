#include "../renderer.h"
#include "util/sampling.h"

#include <iostream>

glm::vec3 SimpleRenderer::traceRay(const Ray& start_ray, RNG& rng) const {

	PathTraceData pt_data;
	pt_data.ray.o = start_ray.o;
	pt_data.ray.dir = start_ray.dir;

	glm::vec3 color(0.0f);

	pt_data.contribution = glm::vec3(0.0f);
	pt_data.exit = false;

	Intersection rec;
	const bool hit_surface = _scene->hasWorld() && _scene->world().intersect(
		pt_data.ray, settings::T_MIN, settings::T_MAX, rec);
	if (hit_surface) {
		closestHit(rec, pt_data, rng);
	}
	else {
		miss(rec, pt_data);
	}

	color += pt_data.contribution;

	return color;
}

