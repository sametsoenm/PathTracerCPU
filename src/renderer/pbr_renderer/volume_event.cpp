#include "../renderer.h"

#include "util/sampling.h"

void PBRRenderer::volumeEvent(RNG& rng, PathTraceData& pt_data) const {
	switch (_fogMode) {
	case settings::FogMode::SINGLE_SCATTER:
		volume_interaction_single(rng, pt_data);
		break;
	case settings::FogMode::MULTIPLE_SCATTER:
		volume_interaction_multiple(rng, pt_data);
		break;
	default:
		volume_interaction_single(rng, pt_data);
	}

}

void PBRRenderer::volume_interaction_multiple(RNG& rng, PathTraceData& pt_data) const {

	const glm::vec3 P = pt_data.ray.o + pt_data.t_sample * pt_data.ray.dir;
	{ // NEE

		const LightSample light_sample = _scene->lights().sample(P, rng.next_vec3());

		const glm::vec3 wl = glm::normalize(light_sample.pos - P);
		const float d = glm::length(light_sample.pos - P);
		const float T = glm::exp(-_scene->fogDensity() * d);

		float V = 1.0f;
		if (Intersection temp;
			_scene->world().intersect(
				Ray(P, wl), settings::T_MIN, d - settings::T_MIN, temp)) {
			V = 0.0f;
		}

		const float phase_func = sampling::sample_isotropic_phase_func();
		const float pdf_NEE = light_sample.pdf;
		const float pdf_phase = sampling::pdf_isotropic_phase_func();

		const float weight = sampling::power_heuristic(pdf_NEE, pdf_phase);

		glm::vec3 radiance =
			V * T * phase_func / pdf_NEE * light_sample.emission;

		pt_data.contribution = weight * radiance;
	}

	// sample new direction
	glm::vec3 wi = sampling::sample_uniform_sphere(rng.next_vec2());

	pt_data.attenuation_step = glm::vec3(1.0f);
	pt_data.last_bounce_specular = false;
	pt_data.prev_scatter_pdf = sampling::pdf_isotropic_phase_func();
	pt_data.prev_hit_pos = P;

	pt_data.ray.o = P;
	pt_data.ray.dir = wi;
}

void PBRRenderer::volume_interaction_single(RNG& rng, PathTraceData& pt_data) const {

	const glm::vec3 P = pt_data.ray.o + pt_data.t_sample * pt_data.ray.dir;
	{ // NEE

		const LightSample light_sample = _scene->lights().sample(P, rng.next_vec3());

		const glm::vec3 wl = glm::normalize(light_sample.pos - P);
		const float d = glm::length(light_sample.pos - P);
		const float T = glm::exp(-_scene->fogDensity() * d);

		float V = 1.0f;
		if (Intersection temp;
			_scene->world().intersect(
				Ray(P, wl), settings::T_MIN, d - settings::T_MIN, temp)) {
			V = 0.0f;
		}

		const float phase_func = sampling::sample_isotropic_phase_func();
		const float pdf_NEE = light_sample.pdf;

		glm::vec3 radiance =
			V * T * phase_func / pdf_NEE * light_sample.emission;

		pt_data.contribution = radiance;
		pt_data.exit = true;
	}
}