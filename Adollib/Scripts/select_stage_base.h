#pragma once

#include <memory>
#include <string>

#include "../Adollib/Scripts/Math/math.h"

namespace Adollib {

	class Collider;
	class Gameobject;

	class Select_stage_base {

	public:
		Gameobject* gameobject = nullptr;

	public:
		void create_selectstage(std::string name, std::string filepath, Vector3 scale = Vector3(1));


	};
}