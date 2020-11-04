#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "ALP__tags.h"
#include "ALP__shapes.h"
#include "ALP__meshcoll_data.h"

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

			unsigned int oncoll_bits; //oncollision enter�Ŏg�p����bit���

			ALP_Collider_shape shape; //�`���
			
			Collider* coll;

			Meshcoll_data meshcoll_data; //MeshCollider�g�p���Ɏg��data vertices,Edges,Facets

		public:

		//oncollision enter
		const bool concoll_enter (const unsigned int tag_name) const;

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
