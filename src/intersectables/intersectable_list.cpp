#include "intersectable_list.h"

bool IntersectableList::intersect(
	const Ray& ray, float t_min, float t_max, Intersection& record) const {

	bool hit_occured = false;
	Intersection temp_record;
	float closest_t = t_max;

	for (const auto obj : _objects) {
		if (obj->intersect(ray, t_min, closest_t, temp_record)) {
			hit_occured = true;
			closest_t = temp_record.t;
			record = temp_record;
		}
	}

	return hit_occured;
}