#pragma once
#include "contact.h"

namespace Adollib {
	namespace physics_function {

		//è’ìÀê∂ê¨(Narrowphase)
		void generate_contact(std::vector<Contacts::Contact_pair>& pairs);
		//void generate_contact(std::vector<Adollib::Plane*> planes, std::vector<Adollib::Box*> boxes, std::vector<Adollib::Sphere*> spheres, std::vector<Contacts::Contact_pair>& pairs);

		//è’ìÀê∂ê¨
		bool generate_contact_sphere_plane	(const std::vector<ALP_Collider_mesh>::iterator& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_sphere	(const std::vector<ALP_Collider_mesh>::iterator& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_box	(const std::vector<ALP_Collider_mesh>::iterator& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_mesh	(const std::vector<ALP_Collider_mesh>::iterator& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, Contacts::Contact_pair& pair);

		bool generate_contact_box_plane		(const std::vector<ALP_Collider_mesh>::iterator& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_box		(const std::vector<ALP_Collider_mesh>::iterator& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_mesh		(const std::vector<ALP_Collider_mesh>::iterator& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, Contacts::Contact_pair& pair);

		bool generate_contact_mesh_plane	(const std::vector<ALP_Collider_mesh>::iterator& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, Contacts::Contact_pair& pair);
		bool generate_contact_mesh_mesh		(const std::vector<ALP_Collider_mesh>::iterator& S1, const std::vector<ALP_Collider_mesh>::iterator& S2, Contacts::Contact_pair& pair);

	}
}
