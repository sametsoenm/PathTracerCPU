#include "../renderer.h"

#include <iostream>

void SimpleRenderer::miss(const Intersection& rec, PathTraceData& pt_data) const {

	pt_data.contribution = glm::vec3(0.0f);
	//glm::vec3 c = _scene.environment().envMap.eval(pt_data.ray.dir);
	//// temporarly clamp to reduce fireflies
	//pt_data.contribution = glm::vec3(
	//	glm::min(c.r, 10.f), glm::min(c.g, 10.f), glm::min(c.b, 10.f));
	pt_data.exit = true;
}