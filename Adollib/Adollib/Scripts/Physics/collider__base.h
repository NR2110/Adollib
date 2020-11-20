#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <unordered_map>
#include "../Math/math.h"
#include "../Object/component.h"

#include "ALP__meshcoll_data.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

namespace Adollib {

	namespace Physics_function {
		//::: ŒvZ‚È‚Ç‚Ég‚¤î•ñ :::
		struct Collider_data {
			Vector3 local_position = Vector3(0,0,0);
			Quaternion local_orientation = quaternion_identity();
			Vector3 local_scale = Vector3(1, 1, 1);

			Vector3 half_size;

			Physics_function::ALP_Collider_shape shape = Physics_function::ALP_Collider_shape::None; //Œ`î•ñ

			Meshcoll_data meshcoll_data; //meshcollider—pdata
		};
		struct Physics_data {
			float inertial_mass = 0; //¿—Ê
			float drag = 0; //‹ó‹C’ïR
			float anglar_drag = 0; //‹ó‹C’ïR
			float dynamic_friction = 0; //“®–€C
			float static_friction = 0; //Ã–€C
			float restitution = 0;	 //”½”­ŒW”

			bool is_fallable = 0; //—‚¿‚È‚¢
			bool is_kinematic = 0;//‰e‹¿‚¤‚¯‚È‚¢(fall‚Í‚·‚é)
			bool is_moveable = 0; //“®‚©‚È‚¢
			bool is_hitable = 0;  //Õ“Ë‚µ‚È‚¢
		};
		//:::::::::::::::::::::::::

		struct ColliderPhysics_itrs {
			std::list<ALP_Collider>::iterator ALPcollider_itr;
			std::list<ALP_Physics>::iterator ALPphysics_itr;

			std::list<Collider*>::iterator coll_itr;

		};

	}

	class Collider : public Component {

	public:
		//::: tagŠÖŒW ::::::::
		u_int tag = 0; //©g‚Ìtag(bit)
		u_int not_hitable_tags = 0; //Õ“Ë‚µ‚È‚¢tags

		//::: unity‚Ìphysics•”•ª •ª‚¯‚é•K—v‚È‚ñ‚Ä‚È‚¢‚â‚ë ::::
		Physics_function::Physics_data physics_data;

	protected:
		std::list<Physics_function::ALP_Collider>::iterator ALPcollider_itr;
		std::list<Physics_function::ALP_Physics>::iterator ALPphysics_itr;

		std::list<Collider*>::iterator coll_itr;//©g‚Ö‚Ìitr

	private:
		bool removed = false; //“ñd‚Éremove‚ğ‚·‚é‚Ì‚ğ–h‚®

	public:
		//on collision enter
		const bool concoll_enter(const unsigned int tag_name) const;

		//•ÀiˆÚ“®‚É—Í‚ğ‰Á‚¦‚é
		void add_force(const Vector3& force);

		//Šp‰ñ“]‚É—Í‚ğ‰Á‚¦‚é
		void add_torque(const Vector3& force);

		//ŒvZ—p‚Ìstruct
		virtual Physics_function::Collider_data get_Colliderdata() const = 0;
		virtual Physics_function::Meshcoll_data get_Meshdata() { return Physics_function::Meshcoll_data(); };

		const Physics_function::Physics_data get_Physicsdata() const {
			//Physics_function::Physics_data data;
			//data.inertial_mass = inertial_mass;
			//data.drag = drag;
			//data.anglar_drag = anglar_drag;
			//data.dynamic_friction = dynamic_friction;
			//data.static_friction = static_friction;
			//data.restitution = restitution;

			//data.is_fallable = is_fallable;
			//data.is_kinematic = is_kinematic;
			//data.is_moveable = is_moveable;
			//data.is_hitable = is_hitable;
			//return data;
			return physics_data;
		};

		//‚±‚Ìcollider‚ğmanager‚©‚çíœ
		void remove_collider();

	public:
		void awake() override;

		void finalize() override;

	};
}