#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"

namespace Adollib {
	class Collider;

	struct Contacted_data {
		Collider* coll; //�����collider
		float penetrate = 0; //�ђʗ�
		Physics_function::Vector3 normal; //�Փ˖@�� world���W�n
		Physics_function::Vector3 contacted_pointA; //���g��GO���W�n�̏Փ˓_
		Physics_function::Vector3 contacted_pointB; //�����GO���W�n�̏Փ˓_
	};
}