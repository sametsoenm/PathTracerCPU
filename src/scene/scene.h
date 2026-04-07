#pragma once

#include "intersectables/intersectable_list.h"
#include "intersectables/sphere.h"
#include "intersectables/triangle.h"
#include "camera/camera.h"


class Scene {
public:
	Scene() {}

	explicit Scene(settings::SceneType type);

	void add_object(const std::shared_ptr<Intersectable>& obj);
	void buildBvh();
	void useBVH(const bool b);

	const Intersectable& world() const { return *_world; }
	const LightList& lights() const { return _lights; }
	const EnvMapOption& environment() const { return _environment; }
	const Camera& cam() const { return _cam; }
	Camera& cam() { return _cam; }

	bool hasWorld() const { return _world != nullptr; }
	bool envMapEnabled() const { return _envMapEnabled; }
	void setEnvMapEnabled(bool b) { _envMapEnabled = b; }
	bool fogEnabled() const { return _fogEnabled; }
	void setFogEnabled(bool b) { _fogEnabled = b; }
	float fogDensity() const { return _fogDensity; }
	void setFogDensity(float mu_t) { _fogDensity = mu_t; }
	void loadEnvMap(std::string name);

private:
	void sphere_scene();
	void cornell_box_scene();
	void og_scene();
	void suzanne_scene();

	void load_obj(const std::string& path,
		const std::shared_ptr<Material>& defaultMaterial,
		const glm::vec3& translation,
		const glm::vec3& scale);


private:
	IntersectableList _objects; // Linear
	std::shared_ptr<Intersectable> _world = nullptr; // BVH
	LightList _lights;
	EnvMapOption _environment;
	Camera _cam;

	bool _bvhEnabled = true;
	bool _envMapEnabled = settings::ENV_MAP;
	bool _fogEnabled = settings::FOGGY;
	float _fogDensity = settings::FOG_DENSITY; // mu_t
};

struct SceneEntry {
	std::string name;
	std::unique_ptr<Scene> scene;
};