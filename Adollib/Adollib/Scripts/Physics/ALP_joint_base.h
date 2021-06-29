#pragma once

#include "../Math/math.h"
#include "contact.h"
#include "../Scene/scene_list.h"

namespace Adollib {

	namespace Physics_function {
		class ALP_Collider;

		class ALP_Joint_base {
		public:
			Scenelist this_scene = Scenelist::scene_null; //����joint�����݂��Ă���scene

			float bias = 0; // �S���̋����̒����l
			ALP_Collider* colliderA = nullptr; // ����A�ւ̃C���f�b�N�X
			ALP_Collider* colliderB = nullptr; // ����B�ւ̃C���f�b�N�X
			Vector3 anchorA; // ����A�̃��[�J�����W�n�ɂ�����ڑ��_
			Vector3 anchorB; // ����B�̃��[�J�����W�n�ɂ�����ڑ��_
			Contacts::Constraint constraint; // �S��

			virtual void vvv() = 0;
		};



	}
}