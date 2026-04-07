#pragma once

#include "intersectable.h"

#include <vector>
#include <memory>

using std::shared_ptr;
using std::make_shared;

class IntersectableList : public Intersectable {
public:

	IntersectableList() {}

	explicit IntersectableList(shared_ptr<Intersectable> __obj) {
		add(__obj);
	}

	explicit IntersectableList(std::vector<shared_ptr<Intersectable>> __objects) {
		_objects = __objects;
	}

	bool intersect(
		const Ray& ray, float t_min, float t_max, Intersection& record) const override;
	bool bbox(AABB& box) const override { return false; } // TODO

	void add(shared_ptr<Intersectable> obj) { _objects.push_back(obj); }
	void clear() { _objects.clear(); }

	const std::vector<shared_ptr<Intersectable>>& objects() const { return _objects; }

private:
	std::vector<shared_ptr<Intersectable>> _objects;
};