#pragma once

#include "../Adollib/Scripts/Object/component.h"

namespace Adollib {
	class Collider;
	class Collider_comp : public Component {

	private:
		std::shared_ptr<Collider> coll = nullptr;

	public:
		std::shared_ptr<Collider> get_collider() { return coll; };

	public:

		void awake() override;
		void update() override;
		void finalize() override;
		void Update_hierarchy() override;

	};

}