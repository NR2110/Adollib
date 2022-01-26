#include "collider_shape.h"

#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;


void Collider_shape::update_world_trans(const Vector3& GO_Wposiiton, const Quaternion& GO_Worientation, const Vector3& GO_Wscale)
{
	old_world_position_ = world_position_;
	old_world_orientation_ = world_orientation_;
	old_world_scale_ = world_scale_;

	world_position_ = GO_Wposiiton + vector3_quatrotate(local_position * GO_Wscale, GO_Worientation);
	//world_orientation_ = (*coll_itr)->gameobject->get_world_orientate() * local_orientation;
	world_orientation_ = local_orientation * GO_Worientation;
	world_scale_ = GO_Wscale * local_scale;

	bool is_chenged_position = (old_world_position_ != world_position_) ? true : false;
	bool is_chenged_orientation = (old_world_orientation_ != world_orientation_) ? true : false;
	bool is_chenged_scale = (old_world_scale_ != world_scale_) ? true : false;

	if (is_chenged_position || is_chenged_orientation || is_chenged_scale
		) {
		Physics_manager::add_moved(ALPcollider_ptr);
	}

};

const u_int Collider_shape::get_tag() const {
	return ALPcollider_ptr->tag;
}

const u_int Collider_shape::get_ignore_tags() const {
	return ALPcollider_ptr->ignore_tags;
}