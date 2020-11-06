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

			Vector3 half_size; //���ۂ�size = half_size * local_scale * GO.world_scale

			//::: GO�ɓn�����߂̃o�b�t�@ :::::::
			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			//::: oncoll_enter :::::::
			u_int oncoll_bits; //oncollision enter�Ŏg�p����bit���
			u_int oncoll_check_bits; //on collision enter���s��tag�̏��(�݂��ɏՓ˂��Ȃ�����oncollenter���K�v�ȏꍇ)

			//::: tag�֌W :::::::::::::
			u_int tag; //���g��tag(bit)
			u_int not_hitable_tags = 0; //�Փ˂��Ȃ�tags

			//::: 14-DOP ::::::::::
			DOP::DOP_14	dop14;	//14DOP �f�[�^
			DOP::DOP_14	dopbase; //meshcollider�p �ǂݍ��ݎ���dop�f�[�^ meshcolider�͂�������DOP�𐶐�

			//:::
			ALP_Collider_shape shape; //�`���

			//::: Component���A�^�b�`���ꂽCollider�ւ̃C�e���[�^ :::
			std::list<Collider*>::iterator coll_itr;

			//::: Physics�ւ̃C�e���[�^ :::
			std::list<ALP_Physics>::iterator ALPphysics;

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

			//::: collider:Component �� local_position�Ȃǂ��ύX���ꂽ�Ƃ��ɌĂԂ��� :::
			// collider::get_Colliderdata()����local_pos�Ȃǂ̏����X�V
			void refresh_ALP_from_data();

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
