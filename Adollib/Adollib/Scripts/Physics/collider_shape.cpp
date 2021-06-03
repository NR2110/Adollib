#include "ALP__physics_manager.h"
#include "collider_shape.h"
using namespace Adollib;

using namespace Physics_function;


void Collider_shape::update_world_trans(const Vector3& GO_Wposiiton, const Quaternion& GO_Worientation, const Vector3& GO_Wscale,
	bool& is_chenged_position, bool& is_chenged_orientation, bool& is_chenged_scale
)
{
	old_world_position_ = world_position_;
	old_world_orientation_ = world_orientation_;
	old_world_scale_ = world_scale_;


	world_position_ = GO_Wposiiton + vector3_quatrotate(local_position * GO_Wscale, world_orientation());
	//world_orientation_ = (*coll_itr)->gameobject->get_world_orientate() * local_orientation;
	world_orientation_ = local_orientation * GO_Worientation;
	world_scale_ = GO_Wscale * local_scale;

	is_chenged_position = (old_world_position_ != world_position_) ? true : false;
	is_chenged_orientation = (old_world_orientation_ != world_orientation_) ? true : false;
	is_chenged_scale = (old_world_scale_ != world_scale_) ? true : false;

	if (is_chenged_position || is_chenged_orientation || is_chenged_scale
		) {
		Phyisics_manager::add_moved(ALPcollider_ptr);
	}

};

const u_int Collider_shape::get_tag() const {
	return ALPcollider_ptr->get_tag();
}

const u_int Collider_shape::get_ignore_tags() const {
	return ALPcollider_ptr->get_ignore_tags();
}