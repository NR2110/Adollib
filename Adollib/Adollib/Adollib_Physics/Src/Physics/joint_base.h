#pragma once

#include "../Math/math.h"

#include <vector>
#include "ALP_anchor.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;
	}

	class Joint_base {
	public:
		Joint_base(Collider* l_colliderA_comp, Collider* l_colliderB_comp, Physics_function::ALP_Joint* l_ALPjoint)
			: ALPjoint(l_ALPjoint)
		{
			collider_comp[0] = l_colliderA_comp;
			collider_comp[1] = l_colliderB_comp;
		};

		float shrink_bias = 0.1f; // �S���̋����̒����l(�k)
		float stretch_bias = 0.1f; // �S���̋����̒����l(�L)
		float slop = 0.003f; //�S���̋��e�덷
		float offset = 0; //�S���̏ꏊ�̂��� (ex. �z��anchor = 0,0 offset = ���_�̋��� �Ŏ������Ă���)
		float limit_bias = 0.1f;
		//std::vector<Vector3> anchor; // ���̂̃��[�J�����W�n�ɂ�����ڑ��_
		//Vector3 anchor[2]; // ���̂̃��[�J�����W�n�ɂ�����ڑ��_


	protected:
		Anchor anchors[3]; //�ڑ��_(�ő�3��) �eshape��world���W�n

		int anchor_count = 0; //�ڑ��_�̐�

		Collider* collider_comp[2] = { nullptr }; // ���̂�component�ւ̃|�C���^

		Physics_function::ALP_Joint* ALPjoint = nullptr; // �v�Z�pJoint�ւ̃|�C���^

	public:
		Collider* get_colliderA() const { return collider_comp[0]; };
		Collider* get_colliderB() const { return collider_comp[1]; };

		const Anchor* get_anchors()const { return anchors; };
		const int     get_anchors_count()const { return anchor_count; };

		// anchor�̒l��h���N���X����X�V����
		virtual void adapt_anchor() = 0;

		// joint��tensor�ւ̉e����������
		virtual Physics_function::Matrix33 tensor_effect() const = 0;

		// linear,angular speed�ւ̉e�����v�Z����
		virtual void velocity_effect() const = 0;

		// limit�̉e����Ԃ�
		virtual bool limit_effect(Physics_function::Vector3& contactP0, Physics_function::Vector3& contactP1, float& penetrate) const = 0;
		// user���Ă΂Ȃ��悤��!!
		void destroy();
	};

}