#include "ALP__shapes.h"
#include "ALP__physics_manager.h"
using namespace Adollib;

using namespace Physics_function;


void ALP_shape::update_world_trans(const Vector3& GO_Wposiiton, const Quaternion& GO_Worientation, const Vector3& GO_Wscale)
{
	old_world_position_ = world_position_;
	old_world_orientation_ = world_orientation_;
	old_world_scale_ = world_scale_;


	world_position_ = GO_Wposiiton + vector3_quatrotate(local_position * GO_Wscale, world_orientation());
	//world_orientation_ = (*coll_itr)->gameobject->get_world_orientate() * local_orientation;
	world_orientation_ = local_orientation * GO_Worientation;
	world_scale_ = GO_Wscale * local_scale;

	if (old_world_position_ != world_position_ ||
		old_world_orientation_ != world_orientation_ ||
		old_world_scale_ != world_scale_
		)
		Phyisics_manager::add_moved(ALPcollider_ptr);


};
