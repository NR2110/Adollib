#include "shape_croth.h"

#include "ALP__physics_manager.h"
#include "ALP__meshcoll_data.h"

using namespace Adollib;
using namespace Physics_function;

Croth_vertex::Croth_vertex(Physics_function::ALP_Collider* l_ALPcollider_ptr) {
	shape_tag = Physics_function::ALPCollider_shape_type::Sphere;
	ALPcollider_ptr = l_ALPcollider_ptr;
	ALPcollider_ptr->is_adapt_shape_for_copy_transform_gameobject = true;
};

void Croth_vertex::effect_for_copy_transform_to_collider(Vector3& GO_Wposiiton, Quaternion& GO_Worientation, Vector3& GO_Wscale) {

	// transformにvertexの影響を入れる
	GO_Wposiiton = GO_Wposiiton + vector3_quatrotate((mesh_data->vertices.at(vertex_id) + vertex_offset->at(mesh_id).at(vertex_id).first) * GO_Wscale, GO_Worientation);
}

void Croth_vertex::effect_for_copy_transform_to_gameobject(const Vector3& position_amount_of_change, const Quaternion& orientation_amount_of_change) {

	vertex_offset->at(mesh_id).at(vertex_id).first += position_amount_of_change;
	// transformにvertexの影響を入れる
	//GO_Wposiiton = GO_Wposiiton + vector3_quatrotate((mesh_data->vertices.at(vertex_id) + vertex_offset->at(vertex_id)) * GO_Wscale, GO_Worientation);
}
