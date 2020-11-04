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

	namespace physics_function {
		//::: ŒvZ‚Ég‚¤î•ñ :::
		struct Collider_data {
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

			physics_function::ALP_Collider_shape shape;
		};

		struct ColliderPhysicsShape_itrs {
			std::list<Collider*>::iterator coll_itr;

			std::list<ALP_Collider>::iterator ALPcollider_itr;
			std::list<ALP_Physics>::iterator ALPphysics_itr;
			std::list<ALP_Shape>::iterator ALPshape_itr;
		};

	}

	class Collider : public Component {

	public:
		//::: unity‚Ìphysics•”•ª •ª‚¯‚é•K—v‚È‚ñ‚Ä‚È‚¢‚â‚ë ::::
		float inertial_mass; //¿—Ê
		float drag; //‹ó‹C’ïR
		float anglar_drag; //‹ó‹C’ïR
		float dynamic_friction; //“®–€C
		float static_friction; //Ã–€C
		float restitution;	 //”½”­ŒW”

		bool is_fallable; //—‚¿‚È‚¢
		bool is_kinematic;//‰e‹¿‚¤‚¯‚È‚¢(fall‚Í‚·‚é)
		bool is_moveable; //“®‚©‚È‚¢
		bool is_hitable;  //Õ“Ë‚µ‚È‚¢

	private:
		std::list<physics_function::ALP_Collider>::iterator ALPcollider_itr;
		std::list<physics_function::ALP_Physics>::iterator ALPphysics_itr;
		std::list<physics_function::ALP_Shape>::iterator ALPshape_itr;

		std::list<Collider*>::iterator coll_itr;//©g‚Ö‚Ìitr
		
	public:
		//©g‚Ö‚Ìitr‚ğ•Ô‚·
		const physics_function::ColliderPhysicsShape_itrs const get_itrs() const {
			physics_function::ColliderPhysicsShape_itrs ret; 

			ret.ALPcollider_itr = ALPcollider_itr;
			ret.ALPphysics_itr = ALPphysics_itr;
			ret.ALPshape_itr = ALPshape_itr;

			ret.coll_itr = coll_itr;

			return ret;
		};

	public:
		//on collision enter
		const bool concoll_enter(const unsigned int tag_name) const;

		//•ÀiˆÚ“®‚É—Í‚ğ‰Á‚¦‚é
		void add_force(const Vector3& force);

		//Šp‰ñ“]‚É—Í‚ğ‰Á‚¦‚é
		void add_torque(const Vector3& force);

		//ŒvZ—p‚Ìstruct‚É‚Ü‚Æ‚ß‚é
		virtual physics_function::Collider_data get_data() = 0;

	public:
		void awake() override;

		void finalize() override;

	};
}