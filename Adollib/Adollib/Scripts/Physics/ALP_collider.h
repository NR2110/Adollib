#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "../Scene/scene_list.h"

#include "ALP__tags.h"
#include "collider_shape.h"
#include "ALP_world_transform.h"

#include <mutex>

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
				const Scenelist l_scene,
				const u_int l_index
			) :
				this_itr(l_itr), index(l_index), scene(l_scene), coll_ptr(l_collitr), ALPphysics(l_ALPphysics), gameobject(l_go) {};

		private:
			//::: Component���A�^�b�`���ꂽCollider�ւ̃C�e���[�^ :::
			Collider* coll_ptr = nullptr;

			//::: Physics�ւ̃|�C���^ :::
			ALP_Physics* ALPphysics = nullptr;

			//::: �A�^�b�`���ꂽGO�ւ̃|�C���^ :::
			Gameobject* gameobject = nullptr;

			//::: ���g�ւ̃C�e���[�^(remove�p) :::
			std::list<ALP_Collider*>::iterator this_itr{};

			//::: ����Collider�������Ă���joint�ւ̃|�C���^�z�� :::
			std::list<ALP_Joint*> joints{};

			u_int index = 0; //����collider��unique��ID
			Scenelist scene = Scenelist::scene_null; //����collider�����݂���scene

			//::: �A�^�b�`���ꂽshape�̔z�� :::
			std::list<Collider_shape*> shapes{};
			std::list<Collider_shape*> added_shapes{}; // �}���`�X���b�h�p �����̓r���Œǉ����ꂽ�v�f

			//
			std::mutex mtx;

		public:
			// private�ɂ���������........
			//::: oncoll_enter :::::::
			Collider_tagbit oncoll_check_bits = 0; //on collision enter���s��tag�̏��(�݂��ɏՓ˂��Ȃ�����oncollenter���K�v�ȏꍇ)
			Collider_tagbit oncoll_bits = 0; //oncollision enter�Ŏg�p����bit���

			//::: tag ::::::::
			Collider_tagbit tag = 0; //���g��tag(bit)
			Collider_tagbit ignore_tags = 0; //�Փ˂��Ȃ�tags(bit)

		public:
			Collider* get_collptr() const { return coll_ptr; };
			ALP_Physics* get_ALPphysics() const { return ALPphysics; };
			//Gameobject* get_gameobject() const { return gameobject; };

			// ALPphysics_ptr��set
			void set_ALPphysics_ptr(ALP_Physics* ptr) { ALPphysics = ptr; };

			// �}���`�X���b�h������ɂ�����Aadd_collider�������_�ł̓��C����list�ɓ��ꂸbuffer��list�ɓ���邽�� ���g��itr���������Ɍ��܂�Ȃ����� set�֐�������
			void set_this_itr(std::list<ALP_Collider*>::iterator itr) { this_itr = itr; };

		public:
			//[[nodiscard]]

			// ����collider��unique��ID
			u_int get_index() const { return index; };

			// ����collider�����݂���scene
			Scenelist get_scene() const { return scene; };

			// on collision enter���s��tag�̏��
			Collider_tagbit get_oncoll_check_bits() const { return oncoll_check_bits; };

			std::list<ALP_Joint*> get_joints() const { return joints; };

			// �A�^�b�`���ꂽshape�̔z��
			std::list<Collider_shape*> get_shapes() const { return shapes; };
			std::list<Collider_shape*> get_added_shapes() const { return added_shapes; };

			// �Փ˂���collider��tag��ۑ�
			void add_oncoll_bits(Collider_tagbit bit) { oncoll_bits |= bit; };

		public:
			//::: �}���`�X���b�h�ɂ��邽��transform��world����ۑ����� :::
			world_trans transform_start; //���߂̒l
			world_trans transform; //�v�Z���Ă���l (���߂̒l�Ƃ̍����ړ��ʂƂ���gameobject.transform�ɓ����)
			world_trans transform_for_GO; //gameobject�֓n�����߂�transform

			//::: �A�^�b�`���ꂽgameobject���폜���ꂽ�Ƃ� true�ɂ��� �ʃX���b�h�Ȃ̂ō폜����^�C�~���O���قȂ邩��
			bool is_deleted = false;

			//::: added�̔z�񂩂烁�C���̔z���adapt���ꂽ��true�ɂ��� (itr��added���w���̂�main�̔z����h���̂��킩��Ȃ�����)
			bool is_added = false;

		public:
			// added_data��main��data�z��Ɉ����z��
			void adapt_added_data() {
				if (added_shapes.size() == 0)return;

				std::lock_guard <std::mutex> lock(mtx);

				shapes.splice(shapes.end(), std::move(added_shapes));

				added_shapes.clear();
			};


		public:

			//::: ���t���[���ĂԂ��� ::::::
			//oncollision enter ����collider�ƐڐG���Ă�����true
			const bool concoll_enter(const unsigned int tag_name);

			// gameobject��transform����collider��world��Ԃł̏����X�V
			void update_world_trans();

			//���W,�p���ɂ��world���̍X�V
			void integrate(float duration, const Vector3& linear_velocity, const Vector3& angula_velocity, const Vector3& old_linear_velocity,const Vector3& old_angula_velocity);

			// main_thread���X�V�����t���[���ɌĂ΂��
			void reset_data_per_frame();

			// ���t���[���s�� collider::component��tag�Ȃǂ̃f�[�^���R�s�[����
			void adapt_collider_component_data();

			//::: collider::component��tag���l�� :::::::::::::
			Collider_tagbit get_tag() const; //���g��tag(bit)
			Collider_tagbit get_ignore_tags() const; //�Փ˂��Ȃ�tags

			//:::::::::::::::::::::::::::
			// �K���shape���A�^�b�`����
			template<typename T>
			T* add_shape() {
				std::lock_guard <std::mutex> lock(mtx);

				static_assert(std::is_base_of<Collider_shape, T>::value == true, "template T must inherit ALP_shape");

				T* shape = newD T(this);

				added_shapes.emplace_back(shape);
				//adapt_added_data();
				return shape;
			};
			Meshcoll_part* add_mesh_shape(const char* filepath, Physics_function::Meshcollider_data* mesh_data);

			// gameobject��transform����collider��world��Ԃł̏����X�V
			void update_world_trans_contain_added();

			// ����collider��������joint��ǉ�
			void add_joint(ALP_Joint* joint);

			// joint����O���ꂽ
			void remove_joint(ALP_Joint* joint);

			// �Փˏ���ۑ�
			void add_contacted_collider(const Contacts::Contact_pair* pair, u_int num) const;


			//::: mainthread����Ă΂�� :::
			// gameobject��transform�֎��g�̕ێ�����transform������
			void adapt_to_gameobject_transform() const;

			void adapt_transform_for_GO();

			// �q�G�����L�[�`��p
			void Update_hierarchy();

			// manager���玩�g��remove,shape�̍폜,joint�̍폜
			void destroy();


		private:

		};
	}
}
