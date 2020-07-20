#pragma once

//:::::::::::::::::::
// �Փ˔���ŗp����const�ϐ��̐ݒ�
//:::::::::::::::::::

namespace Adollib {

	namespace Contacts {
		static constexpr int Contact_max_per_pair = 4; //��̃y�A�ŏՓ˂̌v�Z������ő�̐� ��{�I�ɂ�4
		static constexpr float Allowable_error = 0.02f;  //����ȏ�߂���Γ���Փ˓_�Ƃ���
		static constexpr float Contact_threrhold_normal = 0.004f; // �Փ˓_��臒l�i�@������)
		static constexpr float Contact_threrhold_tangent = 0.02f;// �Փ˓_��臒l�i���ʏ�j
	}

	namespace physics_function {
		static constexpr float bias = 0.1; //�ђʉ������Ɏg�p����΂˂̋���
		static constexpr float slop = 0.005f; //�ђʂ̋��e�덷

		static float timeStep = 0.016; //delta time���ǂ� �Ƃ肠����eplision_time
	}

}
