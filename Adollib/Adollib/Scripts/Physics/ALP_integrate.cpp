#include "ALP_integrate.h"
#include "ALP__physics_manager.h"
using namespace Adollib;
using namespace physics_function;
using namespace Contacts;

//::: get_dataのdataをColliderに入力 :::
void physics_function::refresh_ALP_from_data(std::list<physics_function::ALP_Collider>& ALP_colliders) {

	std::for_each(ALP_colliders.begin(), ALP_colliders.end(), [](ALP_Collider& coll) {
		coll.refresh_ALP_from_data(); 
		}
	);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region ALP_colliders
//:::::::::::::::::::::::::::::::::::::::::::::::::::

void physics_function::update_world_trans(std::list<physics_function::ALP_Collider>& ALP_colliders){

	std::for_each(ALP_colliders.begin(), ALP_colliders.end(), [](ALP_Collider& coll) {
		coll.update_world_trans(); 
		}
	);
}

void physics_function::solv_resolve(std::list<physics_function::ALP_Collider>& ALP_colliders) {

	std::for_each(ALP_colliders.begin(), ALP_colliders.end(), [](ALP_Collider& coll) {
		coll.solv_resolve(); 
		}
	);
}

void physics_function::resolve_gameobject(std::list<physics_function::ALP_Collider>& ALP_colliders) {

	std::for_each(ALP_colliders.begin(), ALP_colliders.end(), [](ALP_Collider& coll) {
		coll.resolve_gameobject(); 
		}
	);
}


void physics_function::update_dop14(std::list<physics_function::ALP_Collider>& ALP_colliders) {

	std::for_each(ALP_colliders.begin(), ALP_colliders.end(), [](ALP_Collider& coll) {
		coll.update_dop14(); 
		}
	);
}

#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::

//:::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region ALP_phyicses
//:::::::::::::::::::::::::::::::::::::::::::::::::::

//速度などをリセット
void physics_function::resetforce(std::list<physics_function::ALP_Physics>& ALP_physics) {

	std::for_each(ALP_physics.begin(), ALP_physics.end(), [](ALP_Physics& phys) {
		phys.reset_force();
		}
	);
}

//外力による速度などの更新
void physics_function::applyexternalforce(std::list<physics_function::ALP_Physics>& ALP_physics) {

	std::for_each(ALP_physics.begin(), ALP_physics.end(), [](ALP_Physics& phys) {
		phys.apply_external_force(Phyisics_manager::timeStep);
		}
	);
}

void physics_function::integrate(std::list<physics_function::ALP_Physics>& ALP_physics) {

	std::for_each(ALP_physics.begin(), ALP_physics.end(), [](ALP_Physics& phys) {
		phys.integrate(Phyisics_manager::timeStep);
		}
	);
}

#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::