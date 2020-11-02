#include "collider__base.h"

using namespace Adollib;
using namespace Contacts;

#include "ALP__manager.h"
#include "../Object/gameobject.h"
#include "../Main/Adollib.h"
#include "contact.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		#pragma region N
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//on collision enter
bool Collider::concoll_enter(std::string tag_name) {

	ALPcollider_itr->oncoll
};

//•ÀiˆÚ“®‚É—Í‚ð‰Á‚¦‚é
void Collider::add_force(const Vector3& force) {

	ALPphysics_itr->add_force(force);
	return;
};

//Šp‰ñ“]‚É—Í‚ð‰Á‚¦‚é
void Collider::add_torque(const Vector3& force) {

	ALPphysics_itr->add_torque(force);
	return;
};

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void Collider::awake() {
	std::list<Collider*>::iterator C = Rigitbody_manager::add_collider(this);
}
void Collider::finalize() {
	Rigitbody_manager::remove_collider(this);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
