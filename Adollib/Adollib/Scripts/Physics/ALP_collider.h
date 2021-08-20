#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "../Scene/scene_list.h"

#include "ALP__tags.h"
#include "collider_shape.h"

namespace Adollib {

	class Collider;
	class Gameobject;

	namespace Physics_function {

		class ALP_Physics;
		class Meshcoll_part;
		class ALP_Joint;

		namespace Contacts {
			struct Contact_pair;
		}

		//�`���
		class ALP_Collider {
		public:
			//�R���X�g���N�^
			ALP_Collider(
				Gameobject* l_go,
				Collider* l_collitr,
				std::list<ALP_Collider*>::iterator l_itr,
				ALP_Physics* l_ALPphysics,
				Scenelist l_scene,
				u_int l_index
			) :
				gameobject(l_go), coll_ptr(l_collitr), this_itr(l_itr), ALPphysics(l_ALPphysics), scene(l_scene), index(l_index) {};

		private:
			//::: ���g�ւ̃C�e���[�^(remove�p) :::
			std::list<ALP_Collider*>::iterator this_itr;

			//::: ����Collider�������Ă���joint�ւ̃|�C���^�z�� :::
			std::list<ALP_Joint*> joints;

			u_int index = 0; //����collider��unique��ID
			Scenelist scene = Scenelist::scene_null; //����colldier�����݂���scene

			//::: �A�^�b�`���ꂽshape�̔z�� :::
			std::vector<Collider_shape*> shapes;

		public:
			// ���������ׂ�private�ɂ���������........
			//::: oncoll_enter :::::::
			Collider_tagbit oncoll_check_bits = 0; //on collision enter���s��tag�̏��(�݂��ɏՓ˂��Ȃ�����oncollenter���K�v�ȏꍇ)
			Collider_tagbit oncoll_bits = 0; //oncollision enter�Ŏg�p����bit���


		public:
			// ����collider��unique��ID
			const u_int get_index() const { return index; };

			// ����colldier�����݂���scene
			const Scenelist get_scene() const { return scene; };

			// on collision enter���s��tag�̏��
			const Collider_tagbit get_oncoll_check_bits() const { return oncoll_check_bits; };

			// �A�^�b�`���ꂽshape�̔z��
			const std::vector<Collider_shape*> get_shapes() const { return shapes; };

			// �Փ˂���collider��tag��ۑ�
			void add_oncoll_bits(Collider_tagbit bit) { oncoll_bits |= bit; };

		private:
			//::: Component���A�^�b�`���ꂽCollider�ւ̃C�e���[�^ :::
			Collider* coll_ptr;

			//::: Physics�ւ̃|�C���^ :::
			ALP_Physics* ALPphysics = nullptr;

			//::: �A�^�b�`���ꂽGO�ւ̃|�C���^ :::
			Gameobject* gameobject = nullptr;

		public:
			Collider* get_collptr() const { return coll_ptr; };
			ALP_Physics* get_ALPphysics() const { return ALPphysics; };
			Gameobject* get_gameobject() const { return gameobject; };

		public:

			//::: ���t���[���ĂԂ��� ::::::
			//oncollision enter ����collider�ƐڐG���Ă�����true
			const bool concoll_enter(const unsigned int tag_name);

			// gameobject��transform����collider��world��Ԃł̏����X�V
			void update_world_trans();

			//���W,�p���ɂ��world���̍X�V
			void integrate(float duration, Vector3 linear_velocity, Vector3 anglar_velocity, Vector3 old_linear_velocity, Vector3 old_angula_velocity);

			// ���t���[���s��reset
			void reset_data_per_frame();

			//::: �e��tag���l�� :::::::::::::
			const Collider_tagbit get_tag() const; //���g��tag(bit)
			const Collider_tagbit get_ignore_tags() const; //�Փ˂��Ȃ�tags

			//:::::::::::::::::::::::::::
			//�K���shape���A�^�b�`����
			template<typename T>
			T* add_shape() {

				static_assert(std::is_base_of<Collider_shape, T>::value == true, "template T must inherit ALP_shape");

				T* shape = newD T(this);

				shapes.emplace_back(shape);
				return shape;
			};
			Meshcoll_part* add_mesh_shape(const char* filepass, Physics_function::Meshcollider_data* mesh_data);


			//����collider��������joint��ǉ�
			void add_joint(ALP_Joint* joint);

			//joint����O���ꂽ
			void remove_joint(ALP_Joint* joint);

			//�Փˏ���ۑ�
			void add_contacted_collider(Contacts::Contact_pair* pair, u_int num);

			//�q�G�����L�[�`��p
			void Update_hierarchy();

			//manager���玩�g��remove & shapes��delete
			void destroy();


		private:
			//::: shape����AABB�̍X�V ::::::
			//void update_AABB();
		};
	}
}
