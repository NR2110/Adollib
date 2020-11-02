#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_shape.h"

namespace Adollib {

	namespace  physics_function {
		class Solverbody;
		class ALP__CollPhysicsShape_data;
	}


	class Collider : public Component {
	public:
		Vector3 local_position;
		Vector3 local_orientation;
		Vector3 local_scale;

		float mass; //質量
		float drag; //空気抵抗
		float anglar_drag; //空気抵抗
		float dynamic_friction; //動摩擦
		float static_friction; //静摩擦
		float restitution;	 //反発係数

		bool is_fallable; //落ちない
		bool is_kinematic; //影響うけない(fallはする)
		bool is_moveable; //動かない

	private:


	private:
		std::list<physics_function::ALP_Collider>::iterator ALPcollider_itr;
		std::list<physics_function::ALP_Physics>::iterator ALPphysics_itr;
		std::list<physics_function::ALP_Shape>::iterator ALPshape_itr;

		
	public:
		//on collision enter
		bool concoll_enter(std::string tag_name);

		//並進移動に力を加える
		void add_force(const Vector3& force);

		//角回転に力を加える
		void add_torque(const Vector3& force);


	public:
		void awake() override;

		void finalize() override;

	};
}