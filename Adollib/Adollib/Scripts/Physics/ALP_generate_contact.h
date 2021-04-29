#pragma once
#include "contact.h"

namespace Adollib {
	namespace Physics_function {

		//è’ìÀê∂ê¨(Narrowphase)
		void generate_contact(std::vector<Contacts::Contact_pair>& pairs);
		//void generate_contact(std::vector<Adollib::Plane*> planes, std::vector<Adollib::Box*> boxes, std::vector<Adollib::Sphere*> spheres, std::vector<Contacts::Contact_pair>& pairs);

		//è’ìÀê∂ê¨
		bool generate_contact_sphere_plane	(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_sphere	(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_box	(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_capsule(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);
		bool generate_contact_sphere_mesh	(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);

		bool generate_contact_box_plane		(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_box		(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_capsule	(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);
		bool generate_contact_box_mesh		(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);

		bool generate_contact_capsule_plane		(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);
		bool generate_contact_capsule_capsule	(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);
		bool generate_contact_capsule_mesh		(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);

		bool generate_contact_mesh_plane	(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);
		bool generate_contact_mesh_mesh		(const ALP_Collider_part* S1, const ALP_Collider_part* S2, Contacts::Contact_pair& pair);

	}
}
