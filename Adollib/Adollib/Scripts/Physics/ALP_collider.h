#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "ALP_tags.h"

namespace Adollib {

	class Collider;

	namespace physics_function {

		class ALP_Collider {
		public:
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

			Vector3	world_position;
			Quaternion	world_orientation;
			Vector3	world_scale;

			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			std::unordered_map<ALP_tags, bool> oncoll_checkmap; //on collision enter��flag�ۑ�
			std::vector<ALP_tags> oncoll_enter_names; //on collision enter��tag�ۑ�

			ALP_Collider_shape shape; //�`���
			
			Collider* coll;


		public:

		//on collision enter
		bool concoll_enter(ALP_tags tag_name);

		//::: ���t���[���ĂԂ��� :::::
		//gameobject�ւ̕ω��ʂ����߂�
		void solv_resolve();

		//gameobject�֕ω��ʂ�n��
		void resolve_gameobject();

		//gameobject��transform����collider��world��Ԃł̏����X�V
		void update_world_trans();

		};
	}
}
