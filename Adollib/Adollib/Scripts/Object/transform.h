#pragma once

#include "../Math/vector.h"
#include "../Math/quaternion.h"

#include <list>
#include <memory>

namespace Adollib {

	//•`‰æ—ptransfome
	class Transfome {
	public:
		//•`‰æ—p‚Ìî•ñ 
		Vector3 position = Vector3(0, 0, 0);
		Quaternion orientation = Quaternion(1, 0, 0, 0);
		Vector3 scale = Vector3(1, 1, 1);

		//e‚©‚ç‚Ì‘Š‘Îî•ñ
		Vector3 local_pos = Vector3(0, 0, 0);
		Quaternion local_orient = Quaternion(1, 0, 0, 0);
		Vector3 local_scale = Vector3(1, 1, 1);
	};

}