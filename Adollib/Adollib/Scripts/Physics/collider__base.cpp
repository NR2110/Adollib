#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Contacts;

//#include "ALP__manager.h"
#include "../Object/gameobject.h"
#include "../Main/Adollib.h"
#include "contact.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Rigitbody
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

const bool Collider::concoll_enter(const unsigned int tag_name) const {
	ALPcollider_itr->concoll_enter(tag_name);
}

void Collider::add_force(const Vector3& force) {
	ALPphysics_itr->add_force(force);
}
void Collider::add_torque(const Vector3& force) {
	ALPphysics_itr->add_torque(force);
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Collider::awake() {
	physics_function::ColliderPhysicsShape_itrs data;

	data = Phyisics_manager::add_collider(this);

	ALPcollider_itr = data.ALPcollider_itr;
	ALPphysics_itr = data.ALPphysics_itr;
	ALPshape_itr = data.ALPshape_itr;

	coll_itr = data.coll_itr;
}
void Collider::finalize() {
	Phyisics_manager::remove_collider(this);
}
