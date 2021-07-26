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
			class Contact_pair;
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
				gameobject(l_go), coll_itr(l_collitr), this_itr(l_itr), ALPphysics(l_ALPphysics), scene(l_scene), index(l_index) {};

		private:
			//::: ���g�ւ̃C�e���[�^(remove�p) :::
			std::list<ALP_Collider*>::iterator this_itr;

			//::: ����Collider�������Ă���joint�ւ̃|�C���^�z�� :::
			std::list<ALP_Joint*> joints;

			u_int index = 0; //����collider��unique��ID
			Scenelist scene = Scenelist::scene_null; //����colldier�����݂���scene

			//::: �A�^�b�`���ꂽshape�̔z�� :::
			std::vector<Collider_shape*> shapes;

			//::: ���g�̊ւ��contact_pair�̏��������o�ɕۑ����邩�ǂ��� :::
			bool is_save_pair_ = false;

			//::: is_save_pair��true�̎� ���g�̊ւ��contact_pair�̏���ۑ�����
			std::vector<std::pair<Contacts::Contact_pair*, u_int>> contacted_colliders;
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

			// ���g�̊ւ��pair�̏��, u_int : Contact_pair::body[u_int] = ����
			const std::vector<std::pair<Contacts::Contact_pair*, u_int>>& get_contacted_colliders() const { return contacted_colliders; };

			// �Փ˂���collider��tag��ۑ�
			void add_oncoll_bits(Collider_tagbit bit) { oncoll_bits |= bit; };

			// �ۑ�����tag��0�Ƀ��Z�b�g
			void reset_data_per_frame() {
				oncoll_bits = 0;
				if (is_save_pair_)contacted_colliders.clear();
			};


		private:
			//::: Component���A�^�b�`���ꂽCollider�ւ̃C�e���[�^ :::
			Collider* coll_itr;

			//::: Physics�ւ̃|�C���^ :::
			ALP_Physics* ALPphysics = nullptr;

			//::: �A�^�b�`���ꂽGO�ւ̃|�C���^ :::
			Gameobject* gameobject = nullptr;

		public:
			Collider* get_collptr() const { return coll_itr; };
			ALP_Physics* get_ALPphysics() const { return ALPphysics; };
			Gameobject* get_gameobject() const { return gameobject; };

		public:

			//::: ���t���[���ĂԂ��� ::::::
			//oncollision enter ����collider�ƐڐG���Ă�����true
			const bool concoll_enter(const unsigned int tag_name);

			// gameobject��transform����collider��world��Ԃł̏����X�V
			void update_world_trans();

			//���W,�p���ɂ��world���̍X�V
			void integrate(float duration, Vector3 linear_velocity, Vector3 anglar_velocity);

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

			//is_save_pair��ύX�����肵�Ȃ�������
			bool is_save_pair() const { return is_save_pair_; };
			bool is_save_pair(bool flag) {
				if (is_save_pair_ == flag)return flag;
				if (is_save_pair_ == false)contacted_colliders.reserve(5); //true�ɂȂ����Ƃ� �Ƃ肠�����K���Ȑ������g���m��
				else {
					contacted_colliders.clear();
					contacted_colliders.resize(0); //false�ɂȂ����Ƃ� �ۑ�����z���������
					contacted_colliders.shrink_to_fit();
				}
				is_save_pair_ = flag;
				return flag;
			};
			//���g�̂������pair�ւ̃|�C���^��ۑ�
			void add_contacted_colliders(Contacts::Contact_pair* pair, u_int num) {
				if (!is_save_pair_)return;
				contacted_colliders.emplace_back(std::pair<Contacts::Contact_pair*, u_int>(pair, num));
			};
			//contacted_colliders�����Z�b�g
			void clear_contacted_colliders() { contacted_colliders.clear(); };

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
