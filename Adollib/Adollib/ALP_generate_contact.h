#pragma once
#include "contact.h"

namespace Adollib {
	namespace physics_function {

		//Õ“Ë¶¬(Narrowphase)
		void generate_contact(std::vector<Contacts::Contact_pair>& pairs);
		//void generate_contact(std::vector<Adollib::Plane*> planes, std::vector<Adollib::Box*> boxes, std::vector<Adollib::Sphere*> spheres, std::vector<Contacts::Contact_pair>& pairs);

		//Õ“Ë¶¬
		bool generate_contact_sphere_sphere(const Sphere& S1, const Sphere& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_plane(const Sphere& S1, const Plane& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_box(const Sphere& S1, const  Box& S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_plane(const Box& S1, const Plane& S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_box(const Box& S1, const  Box& S2, Contacts::Contact_pair& pair);

	}
}
