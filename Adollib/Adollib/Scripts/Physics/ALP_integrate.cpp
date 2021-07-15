#include "ALP_integrate.h"

#include "ALP_collider.h"
#include "ALP_Physics.h"
#include "ALP_joint.h"
#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;


//:::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region ALP_Joint
//:::::::::::::::::::::::::::::::::::::::::::::::::::
void Physics_function::adapt_joint_limit(std::list<Physics_function::ALP_Joint*>& ALP_Joints) {

	std::for_each(ALP_Joints.begin(), ALP_Joints.end(), [](ALP_Joint* joint) {
		}
	);
}
#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::



//:::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region ALP_colliders
//:::::::::::::::::::::::::::::::::::::::::::::::::::

void Physics_function::update_world_trans(std::list<Physics_function::ALP_Collider*>& ALP_colliders){

	std::for_each(ALP_colliders.begin(), ALP_colliders.end(), [](ALP_Collider* coll) {
		coll->update_world_trans();
		}
	);
}

void Physics_function::reset_oncoll_bits(std::list<Physics_function::ALP_Collider*>& ALP_colliders) {

	for (auto& coll : ALP_colliders)coll->reset_oncoll_bits();
}

#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::

//:::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region ALP_phyicses
//:::::::::::::::::::::::::::::::::::::::::::::::::::

//速度などをリセット
void Physics_function::resetforce(std::list<Physics_function::ALP_Physics*>& ALP_physics) {

	std::for_each(ALP_physics.begin(), ALP_physics.end(), [](ALP_Physics* phys) {
		phys->reset_force();
		}
	);
}

//外力による速度などの更新
void Physics_function::applyexternalforce(std::list<Physics_function::ALP_Physics*>& ALP_physics) {

	std::for_each(ALP_physics.begin(), ALP_physics.end(), [](ALP_Physics* phys) {
		phys->apply_external_force(Phyisics_manager::physicsParams.timeStep);
		}
	);
}

void Physics_function::integrate(std::list<Physics_function::ALP_Physics*>& ALP_physics) {

	std::for_each(ALP_physics.begin(), ALP_physics.end(), [](ALP_Physics* phys) {
		phys->integrate(Phyisics_manager::physicsParams.timeStep);
		}
	);
}

#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::