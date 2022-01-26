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

void Physics_function::update_world_trans(const std::list<Scenelist>& active_scenes, std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>>& ALP_colliders) {

	for (const auto& Sce : active_scenes) {
		std::for_each(ALP_colliders[Sce].begin(), ALP_colliders[Sce].end(), [](ALP_Collider* coll) {
			coll->update_world_trans();
			}
		);
	}
}
void Physics_function::update_per_calculate(const std::list<Scenelist>& active_scenes, std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>>& ALP_colliders) {

	for (const auto& Sce : active_scenes) {
		std::for_each(ALP_colliders[Sce].begin(), ALP_colliders[Sce].end(), [](ALP_Collider* coll) {
			coll->update_contacted_collider_data();
			}
		);
	}
}



#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::

//:::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region ALP_phyicses
//:::::::::::::::::::::::::::::::::::::::::::::::::::

//速度などをリセット
void Physics_function::resetforce(const std::list<Scenelist>& active_scenes, std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>& ALP_physics) {

	for (const auto& Sce : active_scenes) {
		std::for_each(ALP_physics[Sce].begin(), ALP_physics[Sce].end(), [](ALP_Physics* phys) {
			phys->reset_force();
			}
		);
	}
}

//外力による速度などの更新
void Physics_function::applyexternalforce(const std::list<Scenelist>& active_scenes, std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>& ALP_physics, const float timeratio_60, const float time_scale) {

	for (const auto& Sce : active_scenes) {
		std::for_each(ALP_physics[Sce].begin(), ALP_physics[Sce].end(), [&](ALP_Physics* phys) {
			phys->apply_external_force(Physics_manager::physicsParams.timeStep / time_scale, timeratio_60);
			}
		);
	}
}

void Physics_function::integrate(const std::list<Scenelist>& active_scenes, std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>& ALP_physics) {

	for (const auto& Sce : active_scenes) {
		std::for_each(ALP_physics[Sce].begin(), ALP_physics[Sce].end(), [](ALP_Physics* phys) {
			phys->integrate(Physics_manager::physicsParams.timeStep);
			}
		);
	}
}

#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::


//:::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region colliders & phyicses
//:::::::::::::::::::::::::::::::::::::::::::::::::::

void Physics_function::copy_transform(const std::list<Scenelist>& active_scenes, std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>>& ALP_colliders, std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>& ALP_physics) {

	for (const auto& Sce : active_scenes) {
		std::for_each(ALP_colliders[Sce].begin(), ALP_colliders[Sce].end(), [](ALP_Collider* coll) {
			coll->copy_transform();
			}
		);
		std::for_each(ALP_physics[Sce].begin(), ALP_physics[Sce].end(), [](ALP_Physics* phys) {
			phys->copy_transform_ptr();
			}
		);
	}
}

void Physics_function::adapt_component_data(const std::list<Scenelist>& active_scenes, std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>>& ALP_colliders, std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>& ALP_physics, std::list<Physics_function::ALP_Joint*>& ALP_joints) {

	for (const auto& Sce : active_scenes) {
		std::for_each(ALP_colliders[Sce].begin(), ALP_colliders[Sce].end(), [](ALP_Collider* coll) {
			coll->adapt_collider_component_data();
			}
		);
		std::for_each(ALP_physics[Sce].begin(), ALP_physics[Sce].end(), [](ALP_Physics* phys) {
			phys->adapt_collider_component_data();
			}
		);
	}
	std::for_each(ALP_joints.begin(), ALP_joints.end(), [](ALP_Joint* joint) {
		joint->adapt_Jointdata();
		}
	);
}

//void Physics_function::adapt_added_data(std::list<Physics_function::ALP_Collider*>& ALP_colliders, std::list<Physics_function::ALP_Physics*>& ALP_physics, std::list<Physics_function::ALP_Joint*>& ALP_joints) {
//
//	std::for_each(ALP_colliders.begin(), ALP_colliders.end(), [](ALP_Collider* coll) {
//		coll->reset_data_per_frame();
//		}
//	);
//	std::for_each(ALP_physics.begin(), ALP_physics.end(), [](ALP_Physics* phys) {
//		phys->reset_data_per_frame();
//		}
//	);
//}

#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::