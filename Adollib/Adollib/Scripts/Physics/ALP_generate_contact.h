#pragma once
#include "contact.h"
#include "collider_shape.h"

namespace Adollib {
	namespace Physics_function {

		//衝突生成(Narrowphase)
		void generate_contact(std::vector<Contacts::Contact_pair>& pairs);
		//void generate_contact(std::vector<Adollib::Plane*> planes, std::vector<Adollib::Box*> boxes, std::vector<Adollib::Sphere*> spheres, std::vector<Contacts::Contact_pair>& pairs);

		//衝突生成
		bool generate_contact_sphere_plane	(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);
		bool generate_contact_sphere_sphere	(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);
		bool generate_contact_sphere_box	(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);
		bool generate_contact_sphere_capsule(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);
		bool generate_contact_sphere_mesh	(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);

		bool generate_contact_box_plane		(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);
		bool generate_contact_box_box		(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);
		bool generate_contact_box_capsule	(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);
		bool generate_contact_box_mesh		(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);

		bool generate_contact_capsule_plane		(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);
		bool generate_contact_capsule_capsule	(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);
		bool generate_contact_capsule_mesh		(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);

		bool generate_contact_mesh_plane	(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);
		bool generate_contact_mesh_mesh		(const Collider_shape* S1, const Collider_shape* S2, Contacts::Contact_pair& pair, bool& is_crossing);

	}
}
