#pragma once

#include <string>

#include "../../Scripts/Math/math.h"

#include "collider_comp.h"

namespace Adollib {
	class Gameobject;

	class Collider_creater {
	public:

		static Collider_comp* create_box(const std::string& name, const Vector3& pos, const Vector3& orient, const Vector3 scale, const Vector3& color, Gameobject* pearent = nullptr);
		static Collider_comp* create_box(const Vector3& pos, const Vector3& orient, const Vector3 scale, const Vector3& color, Gameobject* pearent = nullptr) {
			return create_box("box", pos, orient, scale, color, pearent);
		}

		static Collider_comp* create_sphere(const std::string& name, const Vector3& pos, float r, const Vector3& color, Gameobject* pearent = nullptr);
		static Collider_comp* create_sphere(						 const Vector3& pos, float r, const Vector3& color, Gameobject* pearent = nullptr) {
			return create_sphere("sphere", pos, r, color, pearent);
		}




	};
}