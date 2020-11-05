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

		class ALP_Physics;

		class ALP_Collider {
		public:
			//::: Collider :::::
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

			Vector3	world_position;
			Quaternion	world_orientation;
			Vector3	world_scale;

			Vector3 half_size;

			//::: GO�ɓn�����߂̃o�b�t�@ :::::::
			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			//::: oncoll_enter :::::::
			u_int oncoll_bits; //oncollision enter�Ŏg�p����bit���
			u_int oncoll_check_bits; //on collision enter���s��tag�̏��(�݂��ɏՓ˂��Ȃ�����oncollenter���K�v�ȏꍇ)

			//::: tag�֌W :::::::::::::
			u_int tag; //���g��tag(bit)
			u_int not_hitable_tags; //�Փ˂��Ȃ�tags

			//::: 14-DOP ::::::::::
			DOP::DOP_14	dop14;	//14DOP �f�[�^
			DOP::DOP_14	dopbase; //meshcollider�p �ǂݍ��ݎ���dop�f�[�^ meshcolider�͂�������DOP�𐶐�

			//:::
			ALP_Collider_shape shape; //�`���

			//::: Component���A�^�b�`���ꂽCollider�ւ̃|�C���^ :::
			Collider* coll;

			//::: Physics�ւ̃|�C���^ :::
			ALP_Physics* ALPphysics;

			//:::
			Meshcoll_data meshcoll_data; //MeshCollider�g�p���Ɏg��data vertices,Edges,Facets

		public:

			//oncollision enter
			const bool concoll_enter(const unsigned int tag_name);

			//::: ���t���[���ĂԂ��� :::::
			//gameobject�ւ̕ω��ʂ����߂�
			void solv_resolve();

			// gameobject�֕ω��ʂ�n��
			void resolve_gameobject();

			// gameobject��transform����collider��world��Ԃł̏����X�V
			void update_world_trans();

		public:
			// DOP_14�f�[�^�̍X�V
			void update_dop14();

		private:
			void update_dop14_as_sphere();
			void update_dop14_as_box();
			void update_dop14_as_plane();
			void update_dop14_as_mesh();
		};
	}
}
