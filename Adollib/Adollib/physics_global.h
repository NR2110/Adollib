#pragma once

//:::::::::::::::::::
// �Փ˔���ŗp����const�ϐ��̐ݒ�
//:::::::::::::::::::

namespace Adollib {

	struct physics_g {
		static constexpr int  Contact_max_per_pair = 4; //��̃y�A�ŏՓ˂̌v�Z������ő�̐� ��{�I�ɂ�4
		static constexpr float Allowable_error = 0.02f;  //����ȏ�߂���Γ���Փ˓_�Ƃ���
		static constexpr float Contact_threrhold_normal = 0.004f; // �Փ˓_��臒l�i�@������)
		static constexpr float Contact_threrhold_tangent = 0.02f;// �Փ˓_��臒l�i���ʏ�j

		static float bias; //�ђʉ������Ɏg�p����΂˂̋���
		static float slop; //�ђʂ̋��e�덷

		static float timeStep; //delta time���ǂ� �Ƃ肠����eplision_time
		static constexpr float gravity = 8.9f * 30;

		static int accuracy; //�Փˉ����̌J��Ԃ��� 1�`
	};

}
