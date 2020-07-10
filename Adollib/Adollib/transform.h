#pragma once

#include "vector.h"
#include "quaternion.h"

#include <list>
#include <memory>

namespace Adollib {

	//•`‰æ—ptransfome
	class Transfome {
	public:
		//•`‰æ—p‚Ìî•ñ 
		vector3 position = vector3(0, 0, 0);
		quaternion orientation = quaternion(1, 0, 0, 0);
		vector3 scale = vector3(1, 1, 1);

		//e‚©‚ç‚Ì‘Š‘Îî•ñ
		vector3 local_pos = vector3(0, 0, 0);
		quaternion local_orient = quaternion(1, 0, 0, 0);
		vector3 local_scale = vector3(1, 1, 1);
	};

}