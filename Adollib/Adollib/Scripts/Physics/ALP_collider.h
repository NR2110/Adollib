#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "../Scene/scene_list.h"

#include "ALP__tags.h"
#include "collider_shape.h"

#include "ALP_struct_world_transform.h"
#include "ALP_struct_contacted_data.h"

#include <mutex>
#include <vector>

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
			std::list<ALP_Collider*>::iterator this_itr;

			//::: ����Collider�������Ă���joint�ւ̃|�C���^�z�� :::
			std::list<ALP_Joint*> joints;

			u_int index = 0; //����collider��unique��ID
			Scenelist scene = Scenelist::scene_null; //����collider�����݂���scene

			//::: �A�^�b�`���ꂽshape�̔z�� :::
			std::list<Collider_shape*> shapes;
			std::list<Collider_shape*> added_buffer_shapes; // �}���`�X���b�h�p �����̓r���Œǉ����ꂽ�v�f

			//::: is_save_contacted_colls��true�̎� ���g�̊ւ��contact_pair�̏���ۑ����� :::
			int contacted_colliders_num = 0; //�O�̂��̂�ۑ����Ă����������� contacted_colliders_num:���݂̂��� �Ƃ���
			std::vector<Contacted_data> contacted_colliders[2];

			//
			std::mutex mtx;

		public:
			// private�ɂ��������� ����private�ɂ����bit�̕ύX�߂�ǂ����������荇����public
			//::: oncoll_enter :::::::
			Collider_tagbit oncoll_check_bits = 0; //on collision enter���s��tag�̏��(�݂��ɏՓ˂��Ȃ�����oncollenter���K�v�ȏꍇ)
			int oncoll_bits_num = 0; //�O�̂��̂�ۑ����Ă����������� oncoll_bits_num:���݂̂��� �Ƃ���
			Collider_tagbit oncoll_bits[2] = { 0 }; //oncollision enter�ŏՓ˂���bit���

			//::: tag ::::::::
			Collider_tagbit tag = 0; //���g��tag(bit)
			Collider_tagbit ignore_tags = 0; //�Փ˂��Ȃ�tags(bit)

		public:
			Collider* get_collptr() const { return coll_ptr; };
			ALP_Physics* get_ALPphysics() const { return ALPphysics; };

			// ALPphysics_ptr��set
			void set_ALPphysics_ptr(ALP_Physics* ptr) { ALPphysics = ptr; };

			// �}���`�X���b�h������ɂ�����Aadd_collider�������_�ł̓��C����list�ɓ��ꂸbuffer��list�ɓ���邽�� ���g��itr���������Ɍ��܂�Ȃ����� set�֐�������
			void set_this_itr(std::list<ALP_Collider*>::iterator itr) { this_itr = itr; };

			void lock_mtx() { mtx.lock(); };
			void unlock_mtx() { mtx.unlock(); };

		public:
			//[[nodiscard]] /

			// ����collider��unique��ID
			u_int get_index() const { return index; };

			// ����collider�����݂���scene
			Scenelist get_scene() const { return scene; };

			// on collision enter���s��tag�̏��
			Collider_tagbit get_oncoll_check_bits() const { return oncoll_check_bits; };

			std::list<ALP_Joint*> get_joints() const { return joints; };

			// �A�^�b�`���ꂽshape�̔z��
			std::list<Collider_shape*> get_shapes() const { return shapes; };

			std::list<Collider_shape*> get_added_shapes() const { return added_buffer_shapes; };

			// �Փ˂�������Ԃ��B�ʃX���b�h�̂��ߔz��̎Q�Ɠn�����ł��Ȃ� �R�s�[�n���̂��ߑ��p�񐄏�
			std::vector<Contacted_data> get_contacted_collider() const { return contacted_colliders[1 - contacted_colliders_num]; };

			// �Փ˂���collider��tag��ۑ�
			void add_oncoll_bits(Collider_tagbit bit) { oncoll_bits[oncoll_bits_num] |= bit; };

		public:
			//::: �}���`�X���b�h�ɂ��邽��transform��world����ۑ����� :::
			world_trans transform_start; //���߂̒l
			world_trans transform; //�v�Z���Ă���l (���߂̒l�Ƃ̍����ړ��ʂƂ���gameobject.transform�ɓ����)
			world_trans transform_gameobject; //gameobject��transform

			//::: �A�^�b�`���ꂽgameobject���폜���ꂽ�Ƃ� true�ɂ��� �ʃX���b�h�Ȃ̂ō폜����^�C�~���O���قȂ邩��
			bool is_deleted = false;

			//::: added�̔z�񂩂烁�C���̔z���adapt���ꂽ��true�ɂ��� (itr��added���w���̂�main�̔z����h���̂��킩��Ȃ�����)
			bool is_added = false;


			//::: �Փ˂���collider�̏���ۑ����Ă����� :::
			bool is_save_contacted_colls = false;

		public:
			// added_data��main��data�z��Ɉ����z��
			void adapt_added_data() {
				if (added_buffer_shapes.size() == 0)return;

				std::lock_guard <std::mutex> lock(mtx);

				shapes.splice(shapes.end(), std::move(added_buffer_shapes));

				added_buffer_shapes.clear();
			};

		public:

			//::: ���t���[���ĂԂ��� ::::::
			//oncollision enter ����collider�ƐڐG���Ă�����true
			const bool concoll_enter(const unsigned int tag_name);

			// gameobject��transform����collider��world��Ԃł̏����X�V
			void update_world_trans();

			// contacted_data�̕ۑ���̕ύX�Aclear
			void update_contacted_collider_data();

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

				added_buffer_shapes.emplace_back(shape);
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
			void add_contacted_collider(const Contacts::Contact_pair* pair, u_int num);


			//::: mainthread����Ă΂�� :::
			// gameobject��transform�֎��g�̕ێ�����transform������
			void adapt_to_gameobject_transform() const;

			// gameobject��transform���R�s�[����
			void copy_transform_gameobject();

			// �q�G�����L�[�`��p
			void Update_hierarchy();

			// manager���玩�g��remove,shape�̍폜,joint�̍폜
			void destroy();


		private:

		};
	}
}
