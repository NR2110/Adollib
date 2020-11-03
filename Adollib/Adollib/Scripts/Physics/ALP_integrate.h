#pragma once
#include "contact.h"
#include <memory>
namespace Adollib {
	namespace physics_function {

		void resetforce(std::list<Collider*>& coll);

		//�O�͂ɂ�鑬�x�Ȃǂ̍X�V
		void applyexternalforce(std::list<Adollib::Collider*>& coll);

		void integrate(std::list<Adollib::Collider*>& coll);


	}
}