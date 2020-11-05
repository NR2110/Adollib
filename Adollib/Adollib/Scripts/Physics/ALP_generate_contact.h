#pragma once
#include "contact.h"

namespace Adollib {
	namespace physics_function {

		//�Փː���(Narrowphase)
		void generate_contact(std::vector<Contacts::Contact_pair>& pairs);
		//void generate_contact(std::vector<Adollib::Plane*> planes, std::vector<Adollib::Box*> boxes, std::vector<Adollib::Sphere*> spheres, std::vector<Contacts::Contact_pair>& pairs);

		//�Փː���
		bool generate_contact_sphere_plane(const ALP_Collider& S1, const ALP_Collider& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_sphere(const ALP_Collider& S1, const ALP_Collider& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_box(const ALP_Collider& S1, const  ALP_Collider& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_mesh(const ALP_Collider& S1, const ALP_Collider& S2, Contacts::Contact_pair& pair);

		bool generate_contact_box_plane(const ALP_Collider& S1, const ALP_Collider& S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_box(const ALP_Collider& S1, const  ALP_Collider& S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_mesh(const ALP_Collider& S1, const ALP_Collider& S2, Contacts::Contact_pair& pair);

		bool generate_contact_mesh_plane(const ALP_Collider& S1, const ALP_Collider& S2, Contacts::Contact_pair& pair);
		bool generate_contact_mesh_mesh(const ALP_Collider& S1, const ALP_Collider& S2, Contacts::Contact_pair& pair);

	}
}
