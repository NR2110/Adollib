//
//#include "collider_croth_comp.h"
//
//#include <Adollibphysics.h>
//#include "../../Scripts/Object/gameobject.h"
//
//using namespace Adollib;
//
//void Collider_croth_comp::awake() {
//	coll = std::make_shared<Collider_Croth>();
//	auto p_orient_inv = Quaternion(1, 0, 0, 0);
//	if (gameobject->parent() != nullptr)p_orient_inv = gameobject->parent()->transform->orientation.inverse();
//	coll->awake();
//
//	coll->load_file("./DefaultModel/plane_16x16vertex.fbx", false, gameobject, gameobject->world_position(), gameobject->world_orientate(), gameobject->world_scale(), p_orient_inv, true);
//	vertex_offset = coll->get_vertex_offset();
//}
//
//void Collider_croth_comp::update() {
//
//	coll->update();
//
//	Vector3 pearent_Wpos = Vector3(0);
//	Quaternion pearent_Worient = Quaternion(1, 0, 0, 0);
//	Vector3 pearent_Wscale = Vector3(1);
//
//	if (gameobject->parent() != nullptr) {
//		pearent_Wpos = gameobject->parent()->transform->position;
//		pearent_Worient = gameobject->parent()->transform->orientation;
//		pearent_Wscale = gameobject->parent()->transform->scale;
//	}
//
//
//}
//
//void Collider_croth_comp::finalize() {
//	coll->finalize();
//}
//
//void Collider_croth_comp::Update_hierarchy() {
//
//}