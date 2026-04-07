#include "../renderer.h"

#include <iostream>

void PBRRenderer::miss(const Intersection& rec, PathTraceData& pt_data) const {

	if (pt_data.depth == 0 || !_scene->envMapEnabled()) {
		pt_data.contribution = glm::vec3(0.0f);
		pt_data.exit = true;
		return;
	}
	glm::vec3 c = _scene->environment().envMap.eval(pt_data.ray.dir);
	// temporarly clamp to reduce fireflies
	pt_data.contribution = glm::vec3(
		glm::min(c.r, 10.f), glm::min(c.g, 10.f), glm::min(c.b, 10.f));
	pt_data.exit = true;
}