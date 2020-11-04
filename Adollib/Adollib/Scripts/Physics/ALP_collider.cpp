#include "ALP_Collider.h"

#include "collider__base.h"
#include "../Object/gameobject.h"

using namespace Adollib;
using namespace physics_function;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Rigitbody
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
const bool ALP_Collider::concoll_enter (const unsigned int tag_name) const {
	return (oncoll_bits & tag_name);
}

void ALP_Collider::solv_resolve() {
	offset_CollGO_quat = local_orientation.conjugate() * coll->gameobject->get_world_orientate().conjugate() * world_orientation;
	offset_CollGO_pos = world_position - vector3_quatrotate(local_position * coll->gameobject->get_world_scale(), world_orientation) - coll->gameobject->get_world_position();
}

void ALP_Collider::resolve_gameobject() {
	coll->gameobject->transform->local_orient *= offset_CollGO_quat;
	coll->gameobject->transform->local_pos += offset_CollGO_pos;
}

void ALP_Collider::update_world_trans() {
	world_orientation = coll->gameobject->get_world_orientate() * local_orientation;
	world_scale = coll->gameobject->get_world_scale() * local_scale;
	world_position = coll->gameobject->get_world_position() + vector3_quatrotate(local_position * coll->gameobject->get_world_scale(), world_orientation);
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::