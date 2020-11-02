#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "../Object/gameobject_tags.h"
#include "collider__base.h"

namespace Adollib {
	namespace physics_function {

		class ALP_Collider {
		public:
			Vector3 local_position;
			Vector3 local_orientation;
			Vector3 local_scale;

			Vector3	world_position;
			Vector3	world_prientation;
			Vector3	world_scale;

			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			std::unordered_map<GO_Tag, bool>concollflags;

			Collider* coll;

		private:

		//���W,�p���̍X�V
		void integrate(float duration = 1);

		//gameobject�ւ̕ω��ʂ����߂�
		void solv_resolve();

		//gameobject�֕ω��ʂ�n��
		void resolve_gameobject();

		//gameobject��transform����collider��world��Ԃł̏����X�V
		void update_world_trans();

		//�T�C�Y�ύX�ȂǂɑΉ����邽�ߖ��t���[�������e���\���Ȃǂ��X�V
		void update_inertial(const Vector3& size, float density = 1);

		//�I�u�W�F�N�g���������тɌĂяo���@�̂������悢���K���ɖ��t���[���Ă�
		//DOP_14�f�[�^�̍X�V
		void update_dop14();

		};
	}
}
