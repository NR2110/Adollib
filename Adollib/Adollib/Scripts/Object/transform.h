#pragma once

#include "../Math/vector.h"
#include "../Math/quaternion.h"

#include <list>
#include <memory>

namespace Adollib {

	//�`��ptransfome
	class Transfome {
	public:
		//�`��p�̏�� 
		Vector3 position = Vector3(0, 0, 0);
		Quaternion orientation = Quaternion(1, 0, 0, 0);
		Vector3 scale = Vector3(1, 1, 1);

		//�e����̑��Ώ��
		Vector3 local_pos = Vector3(0, 0, 0);
		Quaternion local_orient = Quaternion(1, 0, 0, 0);
		Vector3 local_scale = Vector3(1, 1, 1);
	};

}