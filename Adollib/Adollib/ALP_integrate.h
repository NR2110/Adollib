#pragma once
#include "contact.h"
#include <memory>
namespace Adollib {
	namespace physics_function {

		void resetforce(std::list<Collider*>& coll);

		//外力による速度などの更新
		void applyexternalforce(std::list<std::shared_ptr<Adollib::Collider>>& coll);

		void integrate(std::list<std::shared_ptr<Adollib::Collider>>& coll);


	}
}