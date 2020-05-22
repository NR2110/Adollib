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
		vector3 position;
		quaternion orientte;
		vector3 scale;

		//�e����̑��Ώ��
		vector3 local_pos;
		quaternion local_orient;
		vector3 local_scale;

		// �A�^�b�`���ꂽ����collider
		std::list<std::shared_ptr<RigitBody_Transform>> colliders;
	};

	//�Փ˗ptransfome
	class RigitBody_Transform {

		std::shared_ptr<Transfome> pearent; //����collider���A�^�b�`���ꂽTransfome�ւ̃|�C���^

		Rigitbody* rigitbody; //collider�{��

	};
}