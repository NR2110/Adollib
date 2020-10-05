#include "lay_func.h"

using namespace Adollib;
using namespace Lay_func;

bool Lay_func::intersectRayCollider(const Lay& lay, Collider* coll, float& min) {
	if (coll->shape == Collider_shape::shape_box) {
		Box* B = dynamic_cast<Box*>(coll);
		if (B == nullptr)return false;
		return intersectRayBox(lay, *B, min);
	}

	return false;
}

bool Lay_func::intersectRayBox(const Lay& lay, const Box& box, float& minf) {
	minf = 0.0f;
	float maxf = FLT_MAX;

	//lay‚Ìî•ñ‚ğbox‚ÌÀ•WŒn‚É
	Lay lay_l;
	lay_l.dir = vector3_quatrotate(lay.dir, box.world_orientation.conjugate()).unit_vect();
	lay_l.pos = vector3_quatrotate(lay.pos - box.world_position, box.world_orientation.conjugate());

	//ƒXƒ‰ƒu‚Å”»’è
	for (int s = 0; s < 3; s++) {

		float ood = 1.0f / lay_l.dir[s];
		float t1 = (box.dop14.min[s] - lay_l.pos[s]) * ood;
		float t2 = (box.dop14.max[s] - lay_l.pos[s]) * ood;
		if (t1 > t2) {
			float T = t1;
			t1 = t2;
			t2 = T;
		}

		if (minf < t1) minf = t1;
		if (maxf > t2) maxf = t2;

		if (maxf < minf)return false;

	}

	return true;
}