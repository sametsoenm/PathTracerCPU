#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>

#include <vector>
#include <memory>
#include <unordered_map>

#include "util/texture.h"

struct LightSample {
	glm::vec3 pos = glm::vec3{ 0 };
	float pdf = 0.0f;
	glm::vec3 emission = glm::vec3{ 0 };
	glm::vec3 normal = glm::vec3{ 0 };
	bool is_hittable = true;
};

class Light {

public:
	virtual ~Light() = default;

	virtual LightSample sample(const glm::vec3& pos, const glm::vec3& u) const = 0;
	virtual float pdf(
		const glm::vec3& hit_pos,
		const glm::vec3& light_pos,
		const glm::vec3& light_normal) const = 0;
	virtual glm::vec3 emission() const = 0;

	// not in use atm
	virtual float pdf_Li(
		const glm::vec3& hit_pos,
		const glm::vec3& light_pos,
		const glm::vec3& light_normal) const = 0;
};

class AreaLight : public Light {

public:
	AreaLight() :
		_origin(glm::vec3(0.0f, 1.0f, 0.0f)),
		_u(glm::vec3(1.0f, 0.0f, 0.0f)),
		_v(glm::vec3(0.0f, -1.0f, 0.0f)),
		_emission(glm::vec3(5.0f)) {}

	AreaLight(
		const glm::vec3& __origin,
		const glm::vec3& __u,
		const glm::vec3& __v,
		const glm::vec3& __emission) :
		_origin(__origin),
		_u(__u),
		_v(__v),
		_emission(__emission) {
	}


	glm::vec3 normal() const {
		return glm::normalize(glm::cross(_u, _v));
	}

	LightSample sample(const glm::vec3& pos, const glm::vec3& u) const override;
	float pdf(
		const glm::vec3& hit_pos,
		const glm::vec3& light_pos,
		const glm::vec3& light_normal) const override;

	float pdf_Li(
		const glm::vec3& hit_pos,
		const glm::vec3& light_pos,
		const glm::vec3& light_normal) const override;

	glm::vec3 emission() const override {
		return _emission;
	}

	bool contains(const glm::vec3& p) const;

	float area() const {
		return glm::length(glm::cross(_u, _v));
	}

	void setOrigin(glm::vec3 __origin) { 
		_origin = __origin; 
	}

private:
	glm::vec3 _origin;
	glm::vec3 _u;
	glm::vec3 _v;
	glm::vec3 _emission;
};

class LightList : public Light {
public:

	LightList() {}

	explicit LightList(std::shared_ptr<Light> __light) {
		add(__light);
	}

	LightSample sample(const glm::vec3& pos, const glm::vec3& u) const override;
	float pdf_Li(
		const glm::vec3& hit_pos,
		const glm::vec3& light_pos,
		const glm::vec3& light_normal) const override;
	float pdf(
		const glm::vec3& hit_pos,
		const glm::vec3& light_pos,
		const glm::vec3& light_normal) const override {
		return 0.0f; //temporary
	} 
	glm::vec3 emission() const override { return glm::vec3(0.0f); } // temporary

	void add(std::shared_ptr<Light> light) {
		_lights.push_back(light);
	}
	void clear() { _lights.clear(); }

	const std::vector<std::shared_ptr<Light>>& lights() const {
		return _lights;
	}

	size_t size() const {
		return _lights.size();
	}

private:
	std::vector<std::shared_ptr<Light>> _lights;
};

class EnvironmentLight : public Light {

public:
	EnvironmentLight() {
		_texture.load("assets/envmap01.hdr", false, texture::ColorSpace::LINEAR);
	}
	EnvironmentLight(std::string filename) {
		_texture.load(filename, false, texture::ColorSpace::LINEAR);
	}

	LightSample sample(const glm::vec3& pos, const glm::vec3& u) const override;
	float pdf(
		const glm::vec3& hit_pos,
		const glm::vec3& light_pos,
		const glm::vec3& light_normal) const override;

	float pdf_Li(
		const glm::vec3& hit_pos,
		const glm::vec3& light_pos,
		const glm::vec3& light_normal) const override;

	glm::vec3 emission() const override;

	glm::vec3 eval(const glm::vec3& dir) const;
	void setTexture(std::string filename) {
		_texture.load(filename, false, texture::ColorSpace::LINEAR);
	}


private:
	Texture _texture;
};

struct EnvMapOption {
	std::string name;
	EnvironmentLight envMap;
};

namespace light {

	inline std::vector<EnvMapOption> envMaps = {
		{"Indoor", EnvironmentLight("assets/envmap01.hdr")},
		{"Nature", EnvironmentLight("assets/envmap02.hdr")}
	};

	inline int getEnvMapIdx(std::string name) {
		for (int i = 0; i < envMaps.size(); i++) {
			if (name == envMaps[i].name)
				return i;
		}
		return 0;
	}

}