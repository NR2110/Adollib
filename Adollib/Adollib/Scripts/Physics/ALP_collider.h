#pragma once
#include <unordered_map>
#include <string>
#include "../Math/math.h"
#include "ALP__tags.h"
#include "ALP__meshcoll_data.h"

#include "../Scene/scene_list.h"
#include "collider_shape.h"

namespace Adollib {

	class Collider;
	class Gameobject;

	namespace Physics_function {

		class ALP_Physics;
		class ALP_Collider;
		class Meshcoll_part;
		struct ALP_Solverbody;

		class ALP_Collider {
		public:
			//�R���X�g���N�^
			ALP_Collider(
				Gameobject* l_go, std::list<Collider*>::iterator l_collitr, std::list<ALP_Collider*>::iterator l_itr, ALP_Physics* l_ALPphysics, Scenelist l_scene, u_int l_index
			) :
				gameobject(l_go), coll_itr(l_collitr), this_itr(l_itr), ALPphysics(l_ALPphysics), scene(l_scene), index(l_index) {};

		private:
			//::: GO�ɓn�����߂̃o�b�t�@ :::::::
			Quaternion buffer_quat_chang;
			Vector3 buffer_pos_chang;

			//::: ���g�ւ̃C�e���[�^(remove�p) :::
			std::list<ALP_Collider*>::iterator this_itr;

		public:
			u_int index = 0; //����collider�̔ԍ�
			Scenelist scene = Scenelist::scene_null; //����colldier�����݂���scene

			//::: �A�^�b�`���ꂽshape�̔z�� :::
			std::vector<Collider_shape*> shapes;

			//::: oncoll_enter :::::::
			Collider_tagbit oncoll_check_bits = 0; //on collision enter���s��tag�̏��(�݂��ɏՓ˂��Ȃ�����oncollenter���K�v�ȏꍇ)
			Collider_tagbit oncoll_bits = 0; //oncollision enter�Ŏg�p����bit���

		public:
			//����collider�̔ԍ�
			const u_int get_index() const { return index; };

			//����colldier�����݂���scene
			const Scenelist get_scene() const { return scene; };

			//on collision enter���s��tag�̏��
			const Collider_tagbit get_oncoll_check_bits() const { return oncoll_check_bits; };

			//�A�^�b�`���ꂽshape�̔z��
			const std::vector<Collider_shape*> get_shapes() const { return shapes; };

			//�Փ˂���collider��tag��ۑ�
			void add_oncoll_bits(Collider_tagbit bit) { oncoll_bits |= bit; };

			//�ۑ�����tag��0�Ƀ��Z�b�g
			void reset_oncoll_bits() { oncoll_bits = 0; };



		private:
			//::: Component���A�^�b�`���ꂽCollider�ւ̃C�e���[�^ :::
			std::list<Collider*>::iterator coll_itr;

			//::: Physics�ւ̃|�C���^ :::
			ALP_Physics* ALPphysics = nullptr;

			//::: �A�^�b�`���ꂽGO�ւ̃|�C���^ :::
			Gameobject* gameobject = nullptr;
		public:
			std::list<Collider*>::iterator get_collitr() const { return coll_itr; };
			ALP_Physics* get_ALPphysics() const { return ALPphysics; };
			Gameobject* get_gameobject() const { return gameobject; };
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
			// DOP_14�f�[�^�̍X�V
			void update_dop14();

			//���W,�p���ɂ��world���̍X�V
			void integrate(float duration, Vector3 linear_velocity, Vector3 anglar_velocity);

			void set_inertial_mass(const Matrix& m);

			////::: �e��tag���l�� :::::::::::::
			const Collider_tagbit get_tag() const; //���g��tag(bit)
			const Collider_tagbit get_ignore_tags() const; //�Փ˂��Ȃ�tags

			//:::::::::::::::::::::::::::
			//�K���shape���A�^�b�`����
			template<typename T>
			T* add_shape() {

				static_assert(std::is_base_of<Collider_shape, T>::value == true, "template T must inherit ALP_shape");

				T* shape = newD T(this);

				ALPphysics->add_tensor_type(shape->get_tensor_type());

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
