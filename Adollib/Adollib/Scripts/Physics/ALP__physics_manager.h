#pragma once

#include <map>
#include "../Object/component.h"
#include "../Object/gameobject.h"
#include "../Scene/scene.h"

#include "collider.h"
#include "ALP_collider.h"
#include "ALP_physics.h"
#include "joint_base.h"

#include "ALP_broadphase.h"

#include "contact.h"

#include "collider_shape.h"


#include "ALP_joint.h"
#include "joint_base.h"

namespace Adollib
{
	class Collider;

	namespace Physics_function {
		// physics_manager��static�ϐ��̏����l
		struct PhysicsParams_default {
		public:

			static constexpr float gravity = 30; //�d��

			static constexpr float bounce_threshold = 0.01f; //���˕Ԃ��臒l
			static constexpr float sleep_threshold = 0.01f; //sleep��臒l

			static constexpr float contact_allowable_error = 0.02f; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
			static constexpr float contact_threrhold_normal = 0.004f; //�Փ˓_��臒l
			static constexpr float contact_threrhold_tangent = 0.02f;//�Փ˓_��臒l



			static constexpr float bias = 0.15f;//�߂荞�݂𒼂���
			static constexpr float slop = 0.003f;//�Փ˂̋��e�l

			static constexpr int solver_iterations = 10; //�Փ˂̐��x
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
			//float gravity = 9.8f; //�d��
			float gravity = 30; //�d��

			float bounce_threshold = 0.01f; //���˕Ԃ��臒l
			float sleep_threshold = 0.01f; //sleep��臒l

			float contact_allowable_error = 0.01f; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
			float contact_threrhold_normal = 0.01f; //�Փ˓_��臒l
			float contact_threrhold_tangent = 0.002f;//�Փ˓_��臒l

			//float linear_sleep_threrhold = 0.2f; //freeze��臒l
			//float angula_sleep_threrhold = 0.02f; //freeze��臒l
			float linear_sleep_threrhold = 0.6f; //freeze��臒l
			float angula_sleep_threrhold = 0.15f; //freeze��臒l




			float bias = 0.15f;//�߂荞�݂𒼂���
			float slop = 0.003f;//�Փ˂̋��e�l

			int solver_iterations = 5; //�Փ˂̐��x
			bool hit_backfaces_flag = false;//mesh�̌�납��Փ˂��邩

			float timeStep = 0.016f;
			float max_timeStep = 0.032f;

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
			static float frame_count;


			static std::unordered_map<Scenelist, u_int> collider_index_count;

			//�edata�̎��Ԕz��
			static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>> ALP_colliders;
			static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>> ALP_physicses;
			static std::list<Physics_function::ALP_Joint*> ALP_joints;

			static std::vector<Physics_function::Contacts::Contact_pair*> pairs[2];
			static u_int pairs_new_num; //pairs�̂ǂ������V�����Փ˂Ȃ̂�

			//���̃t���[���Ԃŕω���������(broad_phase�p)
			static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> moved_collider;   //������
			static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> added_collider;   //�ǉ����ꂽ

		public:
			//��������physics�̒l
			static Physics_function::PhysicsParams physicsParams;

			static bool is_draw_collider; //COllider�̕\��
			static bool is_draw_dop; //dop�̕\��(14-DOP��\������̂��߂�ǂ���������AABB��\������)
			static bool is_draw_joint; //joint�̕\��

		public:
			struct ColliderPhysics_ptrs {
				ALP_Collider* ALPcollider_ptr = nullptr;
				ALP_Physics* ALPphysics_ptr = nullptr;
			};

			static ColliderPhysics_ptrs add_collider(Collider* coll) {

				Scenelist Sce = coll->gameobject->get_scene();

				ColliderPhysics_ptrs ret;
				ALP_Collider*& ALPcollider_ptr = ret.ALPcollider_ptr;
				ALP_Physics*& ALPphysics_ptr = ret.ALPphysics_ptr;

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
					ALPcollider_ptr = newD Physics_function::ALP_Collider(coll->gameobject, coll, collider_itr, nullptr, Sce, collider_index_count[Sce]);

					//phsics���g��������
					ALPphysics_ptr = newD Physics_function::ALP_Physics(coll->gameobject, physics_itr, ALPcollider_ptr, Sce, collider_index_count[Sce]);

					//collider�̒��g������
					*ALPcollider_ptr = Physics_function::ALP_Collider(coll->gameobject, coll, collider_itr, ALPphysics_ptr, Sce, collider_index_count[Sce]);

					//�C�e���[�^�[��manager�̔z��Ƀ|�C���^��ۑ�
					*collider_itr = ALPcollider_ptr;
					*physics_itr = ALPphysics_ptr;

					//phsics�̏����l�̓���
					ALPphysics_ptr->set_default();
				}

				//�ǉ����ꂽcollider
				added_collider[Sce].emplace_back(ALPcollider_ptr);

				//::: �����l������� :::
				coll->physics_data.inertial_mass = physicsParams.inertial_mass; //����
				coll->physics_data.drag = physicsParams.linear_drag; //��C��R
				coll->physics_data.anglar_drag = physicsParams.anglar_drag; //��C��R
				coll->physics_data.dynamic_friction = physicsParams.dynamic_friction; //�����C
				coll->physics_data.static_friction = physicsParams.static_friction; //�Ö��C
				coll->physics_data.restitution = physicsParams.restitution;	 //�����W��

				coll->physics_data.is_fallable = physicsParams.is_fallable; //�����Ȃ�
				coll->physics_data.is_kinmatic_anglar = physicsParams.is_kinmatic_anglar;//�e�������Ȃ�(fall�͂���)
				coll->physics_data.is_kinmatic_linear = physicsParams.is_kinmatic_linear;//�e�������Ȃ�(fall�͂���)
				coll->physics_data.is_moveable = physicsParams.is_moveable; //�����Ȃ�
				coll->physics_data.is_hitable = physicsParams.is_hitable;  //�Փ˂��Ȃ�

				collider_index_count[Sce]++;

				return ret;
			}

			static ALP_Joint* add_Joint() {
				//static_assert(std::is_base_of<Joint_base, T>::value == true, "template T must inherit ALP_Joint_base");

				//����ۂ̃|�C���^�Řg�����m��(itr���R���X�g���N�^�Ŏg����������)
				ALP_Joint* null_ptr;
				ALP_joints.emplace_back(null_ptr);
				auto itr = ALP_joints.end();
				itr--;

				//�v�Z�p�̋��ʂ̃N���X�𐶐�
				ALP_Joint* ALPjoint = newD ALP_Joint(itr);
				//���g������
				*itr = ALPjoint;

				return ALPjoint;
			};

			static void remove_Joint(std::list<Physics_function::ALP_Joint*>::iterator joint_itr) {
				ALP_joints.erase(joint_itr);
			};

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
			//::::::
			//raycast���s��
			// ray_pos : ray�̎n�_
			// ray_dir : ray�̌���
			// tag : ray�̏Փ˂���tag
			// ray_min
			// ---out---
			// tmin : ray�̍ŋߓ_
			// tmax : ray�̍ŉ��_
			// normal : �Փ˂����ʂ�normal
			// coll : �Փ˂���coiider�ւ̃|�C���^
			// Sce  : Ray�̏Փ˂���collider�̑��݂���scene
			//:::::::
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

			//static bool init();

			// ���t���[���Ă΂��X�V����
			static bool update(Scenelist Sce = Scene::now_scene);

			static bool update_Gui();

			// collider�̕\��
			static bool render_collider(Scenelist Sce = Scene::now_scene);
			// kdop�̕\��
			static bool render_dop(Scenelist Sce = Scene::now_scene);
			// joint�̕\��
			static bool render_joint(Scenelist Sce = Scene::now_scene);

			static void destroy(Scenelist Sce = Scene::now_scene);

		};

	}
}