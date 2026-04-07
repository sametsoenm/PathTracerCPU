#pragma once

#include "scene/scene.h"
#include "framebuffer/framebuffer.h"
#include "rendersettings.h"
#include "util/random.h"

struct PathTraceData {
	glm::vec3 contribution{ 0.0f };
	glm::vec3 attenuation_step{ 1.0f };
	Ray ray;
	size_t depth = 0;
	glm::vec3 prev_hit_pos{ 0.0f };
	float prev_scatter_pdf = 0.0f;

	float t_sample = settings::T_MAX; // sampled distance

	bool last_bounce_specular = false;
	bool missed = false;
	bool exit = false;
};

class Renderer {
public:
	explicit Renderer(Scene* scene);

	virtual settings::RendererType getType() const = 0;
	void render(settings::RenderStrategy strategy 
		= settings::RenderStrategy::INTERACTIVE);
	void resetAccumulation();

	const Framebuffer& buffer() const;
	const Scene& scene() const { return *_scene; }
	Scene& scene() { return *_scene; }
	Camera& cam() { return _scene->cam(); }
	size_t sampleIndex() const { return _sampleIdx; }

	void setScene(Scene* __scene) { _scene = __scene; }

	bool isRendering() const { return _isRendering; }
	void setIsRendering(bool b) { _isRendering = b; }

private:
	void render_single_thread();
	void render_multi_thread_fixed_rows();
	void render_multi_thread_atomic_rows();
	void render_multi_thread_tiles();
	void render_interactive();

protected:
	virtual glm::vec3 traceRay(const Ray& ray, RNG& rng) const = 0;
	virtual void closestHit(const Intersection& rec, PathTraceData& pt_data, RNG& rng) const = 0;
	virtual void miss(const Intersection& rec, PathTraceData& pt_data) const = 0;

	Scene* _scene = nullptr;
	Framebuffer _fb;
	std::vector<glm::vec3> _accum;
	size_t _sampleIdx;

	size_t _tilesX;
	size_t _tilesY;
	size_t _tileCount;

	bool _isRendering = true;
};

class PBRRenderer : public Renderer {
public:
	explicit PBRRenderer(Scene* scene) : Renderer(scene) {}
	settings::RendererType getType() const override { return settings::RendererType::PBR; }

	settings::FogMode fogMode() const { return _fogMode; }
	void setFogMode(settings::FogMode mode) { _fogMode = mode; }

private:
	glm::vec3 traceRay(const Ray& ray, RNG& rng) const override;
	void closestHit(const Intersection& rec, PathTraceData& pt_data, RNG& rng) const override;
	void miss(const Intersection& rec, PathTraceData& pt_data) const override;
	void volumeEvent(RNG& rng, PathTraceData& pt_data) const;

	void volume_interaction_multiple(RNG& rng, PathTraceData& pt_data) const;
	void volume_interaction_single(RNG& rng, PathTraceData& pt_data) const;

	settings::FogMode _fogMode = settings::FogMode::SINGLE_SCATTER;
};

class SimpleRenderer : public Renderer {
public:
	explicit SimpleRenderer(Scene* scene) : Renderer(scene) {}
	settings::RendererType getType() const override { return settings::RendererType::SIMPLE; }

private:
	glm::vec3 traceRay(const Ray& ray, RNG& rng) const override;
	void closestHit(const Intersection& rec, PathTraceData& pt_data, RNG& rng) const override;
	void miss(const Intersection& rec, PathTraceData& pt_data) const override;
};