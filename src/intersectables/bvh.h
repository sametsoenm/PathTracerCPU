#pragma once

#include "intersectable.h"

#include <vector>

class BVHNode : public Intersectable {
public:
	BVHNode() = default;

	BVHNode(std::vector<std::shared_ptr<Intersectable>>& objects,
		size_t start, size_t end);

	bool intersect(
		const Ray& ray, float t_min, float t_max, Intersection& record) const override;
	bool bbox(AABB& box) const override;

private:
	std::shared_ptr<Intersectable> _left;
	std::shared_ptr<Intersectable> _right;
	bool _isLeaf = false;
	std::vector<std::shared_ptr<Intersectable>> _primitives;
	AABB _box;
};