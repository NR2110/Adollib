#pragma once

#include <map>
#include "../Object/component.h"
#include "../Object/gameobject.h"
#include "../Scene/scene.h"

#include "collider.h"
#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_joint_base.h"

#include "ALP_broadphase.h"

#include "contact.h"

#include "collider_shape.h"

namespace Adollib
{
	class Collider;

	namespace Physics_function {
		// physics_manager��static�ϐ��̏����l
		struct PhysicsParams_default {
		public:

			static constexpr float gravity = 9.8f; //�d��

			static constexpr float bounce_threshold = 0.01f; //���˕Ԃ��臒l
			static constexpr float sleep_threshold = 0.01f; //sleep��臒l

			static constexpr float contact_allowable_error = 0.02f; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
			static constexpr float contact_threrhold_normal = 0.004f; //�Փ˓_��臒l
			static constexpr float contact_threrhold_tangent = 0.02f;//�Փ˓_��臒l



			static constexpr float bias = 0.15f;//�߂荞�݂𒼂���
			static constexpr float slop = 0.003f;//�Փ˂̋��e�l

			static constexpr int solver_iterations = 5; //�Փ˂̐��x
			static constexpr bool hit_backfaces_flag = false;//mesh�̌�납��Փ˂��邩

			static constexpr float timeStep = 0.016f;

			//::: Physics�̏����l���� :::

			static constexpr float inertial_mass = 1.f; //����
			static constexpr float linear_drag = 0.1f; //��C��R
			static constexpr float anglar_drag = 0.1f; //��C��R
			static constexpr float dynamic_friction = 0.4f;//�����C
			static constexpr float static_friction = 0.4f; //�Ö��C
			static constexpr float restitution = 0.1f;	 //�����W��

			static constexpr bool is_fallable = true; //�����邩
			static constexpr bool is_kinmatic_anglar = true; //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
			static constexpr bool is_kinmatic_linear = true; //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
			static constexpr bool is_moveable = true;//������
			static constexpr bool is_hitable = true;  //�Փ˂��Ȃ�
			//::::::::
		};

		struct PhysicsParams {
		public:
			float gravity = 9.8f; //�d��

			float bounce_threshold = 0.01f; //���˕Ԃ��臒l
			float sleep_threshold = 0.01f; //sleep��臒l

			float contact_allowable_error = 0.01f; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
			float contact_threrhold_normal = 0.01f; //�Փ˓_��臒l
			float contact_threrhold_tangent = 0.002f;//�Փ˓_��臒l

<<<<<<< HEAD
			float linear_sleep_threrhold = 0.5f;
			float angula_sleep_threrhold = 0.5f;
<<<<<<< HEAD
			float accumeimpulse_sleep_threrhold = 0.1f;
=======
			float linear_sleep_threrhold = 0.8f;
			float angula_sleep_threrhold = 1.0f;
>>>>>>> parent of 863bd14... midphaseに保存された前フレームの情報にアクセスを行うせいでGOを削除すると死ぬ
=======
>>>>>>> parent of 1c341af... sleep わからん




			float bias = 0.15f;//�߂荞�݂𒼂���
			float slop = 0.003f;//�Փ˂̋��e�l

			int solver_iterations = 5; //�Փ˂̐��x
			bool hit_backfaces_flag = false;//mesh�̌�납��Փ˂��邩

			float timeStep = 0.016f;
			float max_timeStep = 0.032;

			//::: Physics�̏����l���� :::

			float inertial_mass = 1.f; //����
			float linear_drag = 0.1f; //��C��R
			float anglar_drag = 0.1f; //��C��R
			float dynamic_friction = 0.4f;//�����C
			float static_friction = 0.4f; //�Ö��C
			float restitution = 0.1f;	 //�����W��

			bool is_fallable = true; //�����邩
			bool is_kinmatic_anglar = true; //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
			bool is_kinmatic_linear = true; //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
			bool is_moveable = true;//������
			bool is_hitable = true;  //�Փ˂��Ȃ�
		};

		class Phyisics_manager
		{
		private:
			static int frame_count;

			//collider_component�̃|�C���^�z��
			static std::unordered_map<Scenelist, std::list<Collider*>> colliders;

			static std::unordered_map<Scenelist, u_int> collider_index_count;

			//�edata�̎��Ԕz��
			static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>> ALP_colliders;
			static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>> ALP_physicses;
			static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Joint_base*>> ALP_joint_base;

			static std::vector<Physics_function::Contacts::Contact_pair> pairs[2];
			static u_int pairs_new_num; //pairs�̂ǂ������V�����Փ˂Ȃ̂�
			static std::vector<Physics_function::Contacts::Collider_2> broad_mid_pair;

			//���̃t���[���Ԃŕω���������(broad_phase�p)
			static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> moved_collider;   //������
			static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> added_collider;   //�ǉ����ꂽ

		public:
			//��������physics�̒l
			static Physics_function::PhysicsParams physicsParams;

			static bool is_draw_collider; //COllider�̕\��
			static bool is_draw_dop; //dop�̕\��(14-DOP��\������̂��߂�ǂ���������AABB��\������)

		public:
			struct ColliderPhysics_ptrs {
				std::list<Collider*>::iterator coll_itr;
				ALP_Collider* ALPcollider_ptr = nullptr;
				ALP_Physics* ALPphysics_ptr = nullptr;
			};

			static ColliderPhysics_ptrs add_collider(Collider* coll) {

				Scenelist Sce = coll->gameobject->get_scene();

				ColliderPhysics_ptrs ret;
				std::list<Collider*>::iterator& coll_itr = ret.coll_itr;
				ALP_Collider*& ALPcollider_ptr = ret.ALPcollider_ptr;
				ALP_Physics*& ALPphysics_ptr = ret.ALPphysics_ptr;

				{
					colliders[Sce].emplace_back(coll);
					coll_itr = colliders[Sce].end();
					coll_itr--;
				}
				{
					//itr���ق������ߋ�̃|�C���^�Řg�������
					Physics_function::ALP_Collider* null_coll = nullptr;
					Physics_function::ALP_Physics* null_phy = nullptr;

					ALP_colliders[Sce].emplace_back(null_coll);
					ALP_physicses[Sce].emplace_back(null_phy);

					auto collider_itr = ALP_colliders[Sce].end();
					collider_itr--;
					auto physics_itr = ALP_physicses[Sce].end();
					physics_itr--;

					//collider�̃A�h���X�������� (ALPphysics,ALPcollider�̃R���X�g���N�^�ɂ��݂��̃A�h���X���K�v�Ȃ���)
					ALPcollider_ptr = newD Physics_function::ALP_Collider(coll->gameobject, coll_itr, collider_itr, nullptr, Sce, collider_index_count[Sce]);

					//phsics���g��������
					ALPphysics_ptr = newD Physics_function::ALP_Physics(coll->gameobject, physics_itr,ALPcollider_ptr, Sce, collider_index_count[Sce]);

					//collider�̒��g������
					*ALPcollider_ptr = Physics_function::ALP_Collider(coll->gameobject, coll_itr, collider_itr, ALPphysics_ptr, Sce, collider_index_count[Sce]);

					//�C�e���[�^�[��manager�̔z��Ƀ|�C���^��ۑ�
					*collider_itr = ALPcollider_ptr;
					*physics_itr = ALPphysics_ptr;

					//phsics�̏����l�̓���
					ALPphysics_ptr->set_default();
				}

				//�ǉ����ꂽcollider
				added_collider[Sce].emplace_back(ALPcollider_ptr);

				//::: �����l������� :::
				(*coll_itr)->physics_data.inertial_mass = physicsParams.inertial_mass; //����
				(*coll_itr)->physics_data.drag = physicsParams.linear_drag; //��C��R
				(*coll_itr)->physics_data.anglar_drag = physicsParams.anglar_drag; //��C��R
				(*coll_itr)->physics_data.dynamic_friction = physicsParams.dynamic_friction; //�����C
				(*coll_itr)->physics_data.static_friction = physicsParams.static_friction; //�Ö��C
				(*coll_itr)->physics_data.restitution = physicsParams.restitution;	 //�����W��

				(*coll_itr)->physics_data.is_fallable = physicsParams.is_fallable; //�����Ȃ�
				(*coll_itr)->physics_data.is_kinmatic_anglar = physicsParams.is_kinmatic_anglar;//�e�������Ȃ�(fall�͂���)
				(*coll_itr)->physics_data.is_kinmatic_linear = physicsParams.is_kinmatic_linear;//�e�������Ȃ�(fall�͂���)
				(*coll_itr)->physics_data.is_moveable = physicsParams.is_moveable; //�����Ȃ�
				(*coll_itr)->physics_data.is_hitable = physicsParams.is_hitable;  //�Փ˂��Ȃ�

				collider_index_count[Sce]++;

				return ret;
			}

			static void remove_Collider(
				const Scenelist Sce,
				std::list<Collider*>::iterator coll_itr)
			{
				colliders[Sce].erase(coll_itr);
			}

			//delete�͂��Ȃ�
			static void remove_ALPcollider(
				const Scenelist Sce,
				std::list<Physics_function::ALP_Collider*>::iterator ALPcoll_itr
			) {
				remove_collider_broad_phase(*ALPcoll_itr);
				ALP_colliders[Sce].erase(ALPcoll_itr);
			}

			//delete�͂��Ȃ�
			static void remove_ALPphysics(
				const Scenelist Sce,
				std::list<Physics_function::ALP_Physics*>::iterator ALPphs_itr
			) {
				ALP_physicses[Sce].erase(ALPphs_itr);
			}

		public:
			static bool ray_cast(
				const Vector3& Ray_pos, const Vector3& Ray_dir,
				u_int tag,
				const float ray_min,
				float& tmin, float& tmax,
				Vector3& normal,
				Collider* coll,
				Scenelist Sce = Scene::now_scene);

			static void add_moved(Physics_function::ALP_Collider* coll, Scenelist Sce = Scene::now_scene) {
				moved_collider[Sce].push_back(coll);
			}

		public:

			static bool init();

			// ���t���[���Ă΂��X�V����
			static bool update(Scenelist Sce = Scene::now_scene);

			static bool update_Gui();

			// collider�̕\��
			static bool render_collider(Scenelist Sce = Scene::now_scene);
			// kdop�̕\��
			static bool render_dop(Scenelist Sce = Scene::now_scene);

			static void destroy(Scenelist Sce = Scene::now_scene);

		};

	}
}