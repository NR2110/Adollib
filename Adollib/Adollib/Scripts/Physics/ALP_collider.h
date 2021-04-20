#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "ALP__tags.h"
#include "ALP__shapes.h"
#include "ALP__meshcoll_data.h"

#include "../Scene/scene_list.h"

namespace Adollib {

	class Collider;

	namespace Physics_function {

		class ALP_Physics;
		class ALP_Collider;

		struct ALP_Collider_mesh {

			//14-DOP
			DOP::DOP_14	dop14;	//14DOP �f�[�^

			//meshcollider�p vertexes,Edges,Facets�Ȃǂ̏��
			std::vector<Physics_function::Meshcollider_mesh>::iterator mesh;

			//�e
			std::list<ALP_Collider>::iterator ALPcollider;
		};

		class ALP_Collider {
		public:
			u_int index = 0; //����collider�̔ԍ�
			Scenelist scene = Scenelist::scene_null; //����colldier�����݂���scene

		public:
			//::: Collider :::::
			Vector3 local_position;
			Quaternion local_orientation;
			Vector3 local_scale;

		private:
			Vector3	world_position_;
			Quaternion	world_orientation_;
			Vector3	world_scale_;

			Vector3 old_world_position_;
			Quaternion old_world_orientation_;
			Vector3 old_world_scale_;

		public:
			const Vector3	world_position()const { return world_position_; };
			const Quaternion	world_orientation()const {return world_orientation_;};
			const Vector3	world_scale()const { return world_scale_; };

			const Vector3	old_world_position()const { return old_world_position_; };
			const Quaternion	old_world_orientation()const { return old_world_orientation_; };
			const Vector3	old_world_scale()const { return old_world_scale_; };
		public:

			Vector3 half_size; //���ۂ�size = half_size * local_scale * GO.world_scale

			//::: oncoll_enter :::::::
			u_int oncoll_bits = 0; //oncollision enter�Ŏg�p����bit���
			u_int oncoll_check_bits = 0; //on collision enter���s��tag�̏��(�݂��ɏՓ˂��Ȃ�����oncollenter���K�v�ȏꍇ)

			//::: tag�֌W :::::::::::::
			u_int tag = 0; //���g��tag(bit)
			u_int nohit_tag = 0; //�Փ˂��Ȃ�tags

			//:::
			ALP_Collider_shape shape = ALP_Collider_shape::None; //�`���

			//::: ����mesh�ɑΉ� �e���b�V����dop,���_���Ȃǂ������ɕۑ������ :::
			std::vector<ALP_Collider_mesh> collider_meshes;

			//::: GO�ɓn�����߂̃o�b�t�@ :::::::
			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			//:::
			Meshcoll_data meshcoll_data; //MeshCollider�g�p���Ɏg��data vertices,Edges,Facets

			//::: Component���A�^�b�`���ꂽCollider�ւ̃C�e���[�^ :::
			std::list<Collider*>::iterator coll_itr;

			//::: Physics�ւ̃C�e���[�^ :::
			std::list<ALP_Physics>::iterator ALPphysics;

			//::: ���g�ւ̃C�e���[�^ :::
			std::list<ALP_Collider>::iterator ALPcollider;
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

			// collider::get_Colliderdata()����local_pos�Ȃǂ̏����X�V
			void refresh_ALP_from_data();

			//::: collider:Component �� local_position�Ȃǂ��ύX���ꂽ�Ƃ��ɌĂԂ��� :::
			// DOP_14�f�[�^�̍X�V
			void update_dop14();

			//���W,�p���ɂ��world���̍X�V
			void integrate(float duration , Vector3 linear_velocity, Vector3 anglar_velocity);

		private:
			void update_dop14_as_sphere();
			void update_dop14_as_box();
			void update_dop14_as_capsule();
			void update_dop14_as_plane();
			void update_dop14_as_mesh();
		};
	}
}
