#pragma once

#include "vector.h"
#include "quaternion.h"
#include "rigit_body.h"

#include <list>
#include <memory>

namespace Adollib {

	class RigitBody_Transform;

	//�`��ptransfome
	class Transfome {
	public:
		//�`��p�̏�� 
		vector3 position = vector3(0, 0, 0);
		quaternion prientation = quaternion(1, 0, 0, 0);
		vector3 scale = vector3(1, 1, 1);

		//�e����̑��Ώ��
		vector3 local_pos = vector3(0, 0, 0);
		quaternion local_orient = quaternion(1, 0, 0, 0);
		vector3 local_scale = vector3(1, 1, 1);

		// �A�^�b�`���ꂽ����collider
		std::list<std::shared_ptr<RigitBody_Transform>> colliders;
	};

	//�Փ˗ptransfome
	class RigitBody_Transform {

		std::shared_ptr<Transfome> pearent; //����collider���A�^�b�`���ꂽTransfome�ւ̃|�C���^

		Rigitbody* rigitbody; //collider�{��

	};
}