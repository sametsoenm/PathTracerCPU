#include "bvh.h"

#include "rendersettings.h"

#include <algorithm>
#include <stdexcept>

BVHNode::BVHNode(std::vector<std::shared_ptr<Intersectable>>& objects,
	size_t start, size_t end) {

	const size_t obj_span = end - start;
	if (obj_span == 0)
		throw std::runtime_error("BVHNode: empty object range");

	AABB box;
	for (size_t i = start; i < end; i++) {
		AABB temp_box;
		if (!objects[i]->bbox(temp_box))
			throw std::runtime_error("BVHNode: object has no bounding box");
		box = i == start ? temp_box : aabb::surrounding_box(box, temp_box);
	}

	int axis = aabb::longest_axis(box);

	auto comparator = [axis](
		const std::shared_ptr<Intersectable>& a,
		const std::shared_ptr<Intersectable>& b) {
		AABB box_a, box_b;
		if (!a->bbox(box_a) || !b->bbox(box_b))
			throw std::runtime_error("BVHNode: object has no bounding box");
		return aabb::box_center_on_axis(box_a, axis)
			< aabb::box_center_on_axis(box_b, axis);
	};
	if (obj_span <= settings::BVH_LEAF_PRIMITIVE_COUNT) {
		_isLeaf = true;
		for (size_t i = start; i < end; i++)
			_primitives.push_back(objects[i]);

		_box = box;
		return;
	}
	else {
		std::sort(objects.begin() + start, objects.begin() + end, comparator);
		size_t mid = start + obj_span / 2;

		_left = std::make_shared<BVHNode>(objects, start, mid);
		_right = std::make_shared<BVHNode>(objects, mid, end);
	}

	AABB left_box, right_box;
	if (!_left->bbox(left_box) || !_right->bbox(right_box))
		throw std::runtime_error("BVHNode: object has no bounding box");

	_box = aabb::surrounding_box(left_box, right_box);
}

bool BVHNode::intersect(
	const Ray& ray, float t_min, float t_max, Intersection& record) const {

	if (!_box.intersect(ray, t_min, t_max))
		return false;

	if (_isLeaf) {
		bool hit_anything = false;
		Intersection temp_rec;
		float closest = t_max;
		for (const auto& obj : _primitives) {
			if (obj->intersect(ray, t_min, closest, temp_rec)) {
				hit_anything = true;
				closest = temp_rec.t;
				record = temp_rec;
			}
		}
		return hit_anything;
	}

	Intersection left_rec, right_rec;
	bool hit_left = _left->intersect(ray, t_min, t_max, left_rec);
	//bool hit_right = _right->intersect(ray, t_min, t_max, right_rec);
	bool hit_right = _right->intersect(
		ray, t_min, hit_left ? left_rec.t : t_max, right_rec);

	if (hit_left && hit_right) {
		record = left_rec.t < right_rec.t ? left_rec : right_rec;
		return true;
	}
	else if (hit_left) {
		record = left_rec;
		return true;
	}
	else if (hit_right) {
		record = right_rec;
		return true;
	}
	return false;

}

bool BVHNode::bbox(AABB& box) const {
	box = _box;
	return true;
}