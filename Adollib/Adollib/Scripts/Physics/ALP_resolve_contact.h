#pragma once
#include "../Shader/compute_shader.h"
#include "contact.h"
#include <memory>
namespace Adollib {
	namespace physics_function {

		//�Փˉ���
		void resolve_contact(std::list<Adollib::Collider*>, std::vector<Contacts::Contact_pair>& pairs);

	}
}