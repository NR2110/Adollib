#pragma once
#include "contact.h"

namespace Adollib {
	namespace physics_function {

		//�Փ˂̊m�F
		//bool check_contact_sphere_sphere(Sphere& S1, Sphere& S2);
		//bool check_contact_sphere_plane(Sphere& S1, Plane& S2);
		//bool check_contact_sphere_box(Sphere& S1, Box& S2);
		//bool check_contact_box_plane(Box& S1, Plane& S2);
		//bool check_contact_box_box(Box& S1, Box& S2);

		//�O�͂ɂ�鑬�x�Ȃǂ̍X�V
		void applyexternalforce(std::vector<Collider*>& coll);

		void integrate(std::vector<Collider*>& coll);


	}
}