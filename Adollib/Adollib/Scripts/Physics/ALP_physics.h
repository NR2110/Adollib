#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"

namespace Adollib {
	namespace physics_function {

		class Physics {
		public:
			float mass; //¿—Ê
			float drag; //‹ó‹C’ïR
			float anglar_drag; //‹ó‹C’ïR
			float dynamic_friction;//“®–€C
			float static_friction; //Ã–€C
			float restitution;	 //”½”­ŒW”

			bool is_fallable; //—‚¿‚È‚¢
			bool is_kinematic; //‰e‹¿‚¤‚¯‚È‚¢(fall‚Í‚·‚é)
			bool is_moveable;//“®‚©‚È‚¢

		private:
			float speed;//debug‚µ‚â‚·‚¢‚æ‚¤‚É
			Vector3 linear_velocity;//•Ài‘¬“x
			Vector3 anglar_velocity; //‰ñ“]‘¬“x
			Matrix inertia_tensor;//Šµ«ƒeƒ“ƒ\ƒ‹

			bool sleep_state; //sleepó‘Ô‚©‚Ìflag

		};
	}
}
