#include "../renderer.h"

#include <glm/exponential.hpp>
#include <util/sampling.h>

#include <iostream>

// unidirectional path with MIS
void PBRRenderer::closestHit(const Intersection& rec, PathTraceData& pt_data, RNG& rng) const {

	if (rec.material->is_emissive()) {

		if (pt_data.depth == 0 || pt_data.last_bounce_specular) {
			pt_data.contribution = rec.material->emission();
			pt_data.exit = true;
			return;
		}

		float pdf_bsdf = pt_data.prev_scatter_pdf;
		float pdf_light = rec.light->pdf(pt_data.prev_hit_pos, rec.pos, rec.normal);

		const float weight = sampling::power_heuristic(pdf_bsdf, pdf_light);
		pt_data.contribution = weight * rec.material->emission();
		pt_data.exit = true;
		return;
	}

	const glm::vec3 N = rec.shading_normal;
	const glm::vec3 Ng = rec.normal;
	const float eps = 1e-4f;
	const glm::vec3 P = rec.pos;
	const glm::vec3 wo = -glm::normalize(pt_data.ray.dir);
	const float T = _scene->fogEnabled() ? glm::exp(-_scene->fogDensity() * rec.t) : 1.0f;

	// NEE
	if (!_scene->lights().lights().empty()) {
		
		const LightSample light_sample = _scene->lights().sample(P, rng.next_vec3());

		const glm::vec3 wl = glm::normalize(light_sample.pos - P);
		const float d = glm::length(light_sample.pos - P);
		const float T_NEE = _scene->fogEnabled() ? glm::exp(-_scene->fogDensity() * d) : 1.0f;

		float V = 1.0f;
		if (Intersection temp;
			_scene->world().intersect(
				Ray(P, wl), settings::T_MIN, d - settings::T_MIN, temp)) {
			V = 0.0f;
		}

		const glm::vec3 bsdf_cos = rec.material->eval_bsdf_cos(wo, wl, rec);

		const float pdf_NEE = light_sample.pdf;
		const float pdf_bsdf = rec.material->pdf(wo, wl, rec);

		const float weight = light_sample.is_hittable 
			? sampling::power_heuristic(pdf_NEE, pdf_bsdf) 
			: 1.0f;
		glm::vec3 radiance 
			= V * T * T_NEE * bsdf_cos * light_sample.emission / pdf_NEE;
		pt_data.contribution = weight * radiance;
	}


	// sample new direction according to bsdf
	glm::vec2 u = rng.next_vec2();
	BSDFSample s = rec.material->sample(wo, rec, u);
	const glm::vec3 offset = eps * Ng * s.sign;

	pt_data.attenuation_step = T * s.contrib;
	pt_data.prev_scatter_pdf = s.pdf;
	pt_data.prev_hit_pos = P;
	pt_data.last_bounce_specular = s.is_delta;

	pt_data.ray.o = P + offset;
	pt_data.ray.dir = s.wi;
}

// unidirectional path with NEE
//void PBRRenderer::closestHit(const Intersection& rec, PathTraceData& pt_data, RNG& rng) const {
//
//	if (rec.material->is_emissive()) {
//		if (pt_data.depth == 0 || pt_data.last_bounce_specular) {
//			pt_data.contribution = rec.material->emission();
//		}
//		else {
//			pt_data.contribution = glm::vec3(0.0f);
//		}
//		pt_data.exit = true;
//		return;
//	}
//
//	const glm::vec3 N = rec.shading_normal;
//	const glm::vec3 Ng = rec.normal;
//	const float eps = 1e-4f;
//	const glm::vec3 P = rec.pos;
//	const glm::vec3 wo = -glm::normalize(pt_data.ray.dir);
//	const float T = _scene->fogEnabled() ? glm::exp(-_scene->fogDensity() * rec.t) : 1.0f;
//
//	{ // NEE
//
//		const LightSample light_sample = _scene->lights().sample(P, rng.next_vec3());
//
//		const glm::vec3 wl = glm::normalize(light_sample.pos - P);
//		const float d = glm::length(light_sample.pos - P);
//
//		float V = 1.0f;
//		if (Intersection temp;
//			_scene->world().intersect(
//				Ray(P, wl), settings::T_MIN, d - settings::T_MIN, temp)) {
//			V = 0.0f;
//		}
//
//		const glm::vec3 bsdf_cos = rec.material->eval_bsdf_cos(wo, wl, rec);
//		const float pdf_NEE = light_sample.pdf;
//		const float T_NEE = _scene->fogEnabled() ? glm::exp(-_scene->fogDensity() * d) : 1.0f;
//
//		glm::vec3 radiance 
//			= V * T * T_NEE / pdf_NEE * bsdf_cos * light_sample.emission;
//
//		pt_data.contribution = radiance;
//	}
//
//	// sample new direction according to bsdf
//	glm::vec2 u = rng.next_vec2();
//	BSDFSample s = rec.material->sample(wo, rec, u);
//	const glm::vec3 offset = eps * Ng * s.sign;
//
//	pt_data.attenuation_step = T * s.contrib;
//	pt_data.last_bounce_specular = s.is_delta;
//
//	pt_data.ray.o = P + offset;
//	pt_data.ray.dir = s.wi;
//}

// only unidirection path tracing
//void PBRRenderer::closestHit(const Intersection& rec, PathTraceData& pt_data, RNG& rng) const {
//	
//	const glm::vec3 wo = -glm::normalize(pt_data.ray.dir);
//
//	if (rec.material->is_emissive()) {
//		glm::vec3 emission = glm::dot(wo, rec.normal) > 0.0f
//			? rec.material->emission()
//			: glm::vec3(0.0f);
//		pt_data.contribution = emission;
//		pt_data.exit = true;
//		return;
//	}
//
//	const float T = _scene->fogEnabled() ? glm::exp(-_scene->fogDensity() * rec.t) : 1.0f;
//	glm::vec2 u = rng.next_vec2();
//	BSDFSample s = rec.material->sample(wo, rec, u);
//	const glm::vec3 offset = 1e-4f * rec.normal * s.sign;
//
//	pt_data.attenuation_step = T * s.contrib;
//	pt_data.ray.o = rec.pos + offset;
//	pt_data.ray.dir = s.wi;
//}

