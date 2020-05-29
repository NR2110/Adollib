#pragma once

#include "vector.h"
#include "quaternion.h"

#include <list>
#include <memory>

namespace Adollib {

	//�`��ptransfome
	class Transfome {
	public:
		vector3 scale = vector3(1, 1, 1);

		//�`��p�̏�� 
		vector3 position = vector3(0, 0, 0);
		quaternion orientation = quaternion(1, 0, 0, 0);
		//�e����̑��Ώ��
		vector3 local_pos = vector3(0, 0, 0);
		quaternion local_orient = quaternion(1, 0, 0, 0);
		vector3 local_scale = vector3(1, 1, 1);
	};

}