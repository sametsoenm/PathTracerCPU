#include "intersectable.h"
#include "light/light.h"

#include "material/lambert_diffuse.h"

class Triangle : public Intersectable, public Light {
public:
	Triangle() : 
		_v0(0.0f), _v1(1.0f, 0.0f, 0.0f), _v2(0.0f, 1.0f, 0.0f),
		_material(std::make_shared<LambertDiffuseMaterial>(glm::vec3(1.0f))) {
		updateGeometry();
	}

	Triangle(
		const glm::vec3& __v0,
		const glm::vec3& __v1,
		const glm::vec3& __v2, 
		std::shared_ptr<Material> __material) :
		_v0(__v0), _v1(__v1), _v2(__v2),
		_uv0(0.0f), _uv1(0.0f), _uv2(0.0f),
		_material(std::move(__material)) {
		updateGeometry();
	}

	Triangle(
		const glm::vec3& __v0,
		const glm::vec3& __v1,
		const glm::vec3& __v2,
		const glm::vec2& __uv0,
		const glm::vec2& __uv1,
		const glm::vec2& __uv2,
		std::shared_ptr<Material> __material) :
		_v0(__v0), _v1(__v1), _v2(__v2),
		_uv0(__uv0), _uv1(__uv1), _uv2(__uv2), 
		_material(std::move(__material)) {
		updateGeometry();
	}

	Triangle(
		const glm::vec3& __v0,
		const glm::vec3& __v1,
		const glm::vec3& __v2,
		const glm::vec2& __uv0,
		const glm::vec2& __uv1,
		const glm::vec2& __uv2,
		const glm::vec3& __n0,
		const glm::vec3& __n1,
		const glm::vec3& __n2,
		std::shared_ptr<Material> __material) :
		_v0(__v0), _v1(__v1), _v2(__v2),
		_uv0(__uv0), _uv1(__uv1), _uv2(__uv2),
		_n0(__n0), _n1(__n1), _n2(__n2), _hasVertexNormals(true),
		_material(std::move(__material)) {
		updateGeometry();
	}

	Triangle(
		const glm::vec3& __v0,
		const glm::vec3& __v1,
		const glm::vec3& __v2,
		const glm::vec3& __n0,
		const glm::vec3& __n1,
		const glm::vec3& __n2,
		std::shared_ptr<Material> __material) :
		_v0(__v0), _v1(__v1), _v2(__v2),
		_uv0(0.0f), _uv1(0.0f), _uv2(0.0f),
		_n0(__n0), _n1(__n1), _n2(__n2), _hasVertexNormals(true),
		_material(std::move(__material)) {
		updateGeometry();
	}

	bool intersect(
		const Ray& ray, float t_min, float t_max, Intersection& record) const override;
	bool bbox(AABB& box) const override;

	LightSample sample(const glm::vec3& pos, const glm::vec3& u) const override;
	float pdf_Li(
		const glm::vec3& hit_pos,
		const glm::vec3& light_pos,
		const glm::vec3& light_normal) const override;

	float pdf(
		const glm::vec3& hit_pos,
		const glm::vec3& light_pos,
		const glm::vec3& light_normal) const override;

	glm::vec3 emission() const override {
		return _material->emission();
	}

	glm::vec3 normal() const {
		return _normalFlat;
	}

	bool contains(const glm::vec3& p) const;

	float area() const {
		return 0.5f * glm::length(glm::cross(_edge1, _edge2));
	}

private:
	void updateGeometry();

	glm::vec3 _v0;
	glm::vec3 _v1;
	glm::vec3 _v2;

	glm::vec2 _uv0;
	glm::vec2 _uv1;
	glm::vec2 _uv2;

	bool _hasVertexNormals = false;
	glm::vec3 _n0; // normals corresponding to vertices
	glm::vec3 _n1;
	glm::vec3 _n2;

	glm::vec3 _edge1;
	glm::vec3 _edge2;
	glm::vec3 _normalFlat;
	glm::vec3 _tangent;

	std::shared_ptr<Material> _material;
};