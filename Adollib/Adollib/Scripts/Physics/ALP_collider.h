#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "ALP__tags.h"
#include "ALP__meshcoll_data.h"

#include "../Scene/scene_list.h"
#include "ALP__shapes.h"

namespace Adollib {

	class Collider;
	class Gameobject;

	namespace Physics_function {

		class ALP_Physics;
		class ALP_Collider;

		class Meshcoll_part;

		enum class ALP_Collider_inertial_shape {
			box,
			sphere
		};


		class ALP_Collider {
		public:
			//�R���X�g���N�^
			ALP_Collider(std::list<ALP_Collider*>::iterator l_itr, Scenelist l_scene, u_int l_index)
				: this_itr(l_itr), scene(l_scene), index(l_index) {};

		private:
			//::: GO�ɓn�����߂̃o�b�t�@ :::::::
			Quaternion offset_CollGO_quat;
			Vector3 offset_CollGO_pos;

			//::: ���g�ւ̃C�e���[�^(remove�p) :::
			std::list<ALP_Collider*>::iterator this_itr;

			u_int index = 0; //����collider�̔ԍ�
			Scenelist scene = Scenelist::scene_null; //����colldier�����݂���scene

		public:
			//����collider�̔ԍ�
			const u_int get_index() const { return index; };

			//����colldier�����݂���scene
			const Scenelist get_scene() const { return scene; };

		public:
			//::: oncoll_enter :::::::
			u_int oncoll_bits = 0; //oncollision enter�Ŏg�p����bit���
			u_int oncoll_check_bits = 0; //on collision enter���s��tag�̏��(�݂��ɏՓ˂��Ȃ�����oncollenter���K�v�ȏꍇ)

			//::: tag�֌W :::::::::::::
			u_int tag = 0; //���g��tag(bit)
			u_int nohit_tag = 0; //�Փ˂��Ȃ�tags

			//::: �A�^�b�`���ꂽshape�̔z�� :::
			std::vector<ALP_shape*> shapes;

			////::: �^��� :::
			ALP_Collider_inertial_shape shape = ALP_Collider_inertial_shape::box;

			//::: �q�I�u�W�F�N�g�̉e�����󂯂邩 �e�����󂯂�ꍇ,�q�I�u�W�F�N�g�̑��΍��W�Ȃǂ͕ύX����Ȃ� :::
			bool is_chiled_relative_fixed = false;

			//::: Component���A�^�b�`���ꂽCollider�ւ̃C�e���[�^ :::
			std::list<Collider*>::iterator coll_itr;

			//::: Physics�ւ̃|�C���^ :::
			ALP_Physics* ALPphysics;

			//::: ���g�ւ̃|�C���^ :::
			ALP_Collider* ALPcollider;

			//::: �A�^�b�`���ꂽGO�ւ̃|�C���^ :::
			Gameobject* gameobject = nullptr;
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



			//:::::::::::::::::::::::::::
			//�K���shape���A�^�b�`����
			template<typename T>
			T* add_shape() {
				//if (dynamic_cast<ALP_shape>(T) == nullptr) assert("ALP_shape���p������class���A�^�b�`���Ă�������");

				T* shape = new T;

				shapes.emplace_back(shape);
				return shape;
			};

			Meshcoll_part* add_mesh_shape(const char* filepass, Physics_function::Meshcollider_data* mesh_data);

			//�q�G�����L�[�`��p
			void Update_hierarchy();

			//manager����N���X�̍폜 & shapes��delete
			void destroy();


		//private:
		//	void update_dop14_as_sphere();
		//	void update_dop14_as_box();
		//	void update_dop14_as_capsule();
		//	void update_dop14_as_plane();
		//	void update_dop14_as_mesh();
		};
	}
}
