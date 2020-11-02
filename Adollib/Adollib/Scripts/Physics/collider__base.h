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

		float mass; //¿—Ê
		float drag; //‹ó‹C’ïR
		float anglar_drag; //‹ó‹C’ïR
		float dynamic_friction; //“®–€C
		float static_friction; //Ã–€C
		float restitution;	 //”½”­ŒW”

		bool is_fallable; //—‚¿‚È‚¢
		bool is_kinematic; //‰e‹¿‚¤‚¯‚È‚¢(fall‚Í‚·‚é)
		bool is_moveable; //“®‚©‚È‚¢

	private:


	private:
		std::list<physics_function::ALP_Collider>::iterator ALPcollider_itr;
		std::list<physics_function::ALP_Physics>::iterator ALPphysics_itr;
		std::list<physics_function::ALP_Shape>::iterator ALPshape_itr;

		
	public:
		//on collision enter
		bool concoll_enter(std::string tag_name);

		//•ÀiˆÚ“®‚É—Í‚ğ‰Á‚¦‚é
		void add_force(const Vector3& force);

		//Šp‰ñ“]‚É—Í‚ğ‰Á‚¦‚é
		void add_torque(const Vector3& force);


	public:
		void awake() override;

		void finalize() override;

	};
}