//#include "ALP__physics_manager.h"
//
//using namespace Adollib;
//
////#include "ALP__manager.h"
//#include "../Object/gameobject.h"
//#include "../Main/Adollib.h"
//#include "contact.h"
//
////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//#pragma region Rigitbody
////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//const bool Collider::concoll_enter(const unsigned int tag_name) const {
//	return ALPcollider_itr->concoll_enter(tag_name);
//}
//
//void Collider::add_force(const Vector3& force) {
//	ALPphysics_itr->add_force(force);
//}
//void Collider::add_torque(const Vector3& force) {
//	ALPphysics_itr->add_torque(force);
//}
//
//void Collider::remove_collider() {
//	if (removed == true)return;
//	Phyisics_manager::remove_collider(coll_itr, ALPcollider_itr, ALPphysics_itr);
//	removed = true;
//}
//
//#pragma endregion
////::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//void Collider::awake() {
//	Physics_function::ColliderPhysics_itrs data;
//
//	data = Phyisics_manager::add_collider(this);
//
//	ALPcollider_itr = data.ALPcollider_itr;
//	ALPphysics_itr = data.ALPphysics_itr;
//	coll_itr = data.coll_itr;
//
//	data.ALPcollider_itr->coll_itr = coll_itr;
//
//	data.ALPcollider_itr->collider_meshes.resize(1);
//	data.ALPcollider_itr->collider_meshes.at(0).ALPcollider = data.ALPcollider_itr;
//}
//
//void Collider::finalize() {
//	remove_collider();
//}
