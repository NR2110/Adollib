#pragma once

#include <map>

#include "collider.h"
#include "ALP_collider.h"
#include "ALP_physics.h"
#include "joint_base.h"

#include "ALP_broadphase.h"

#include "ALP_contact.h"

#include "collider_shape.h"


#include "ALP_joint.h"
#include "joint_base.h"

#include <map>
#include <mutex>
#include <Windows.h>


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
			static constexpr bool is_static = false; //static���m�͏Փ˂��� oncoll_enter���������Ȃ� ���ǌy���Ȃ�
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

			float bias = 0.15f;//�߂荞�݂𒼂���
			float slop = 0.003f;//�Փ˂̋��e�l

			int solver_iteration = 10; //�Փ˂̐��x
			int calculate_iteration = 5; //�v�Z�̐��x

			int sweep_and_prune_divide_value = 100; //SweepAndPrune��X�������̕���

			float timescale = 1.0f;
			float timeStep = inv120;
			//float caluculate_time = inv120; //timeStep������ȏ�̎��ɍX�V������
			float caluculate_time = inv120; //timeStep������ȏ�̎��ɍX�V������
			float max_timeStep = 0.05f;


			//::: Physics�̏����l���� :::
			float inertial_mass = 1.f; //����
			float linear_drag = 0.1f; //��C��R
			float anglar_drag = 0.1f; //��C��R
			float dynamic_friction = 0.8f;//�����C
			float static_friction = 0.8f; //�Ö��C
			float restitution = 0.1f;	 //�����W��
			float linear_sleep_threrhold = 0.2f; //freeze��臒l
			float angula_sleep_threrhold = 0.1f; //freeze��臒l

			bool is_fallable = true; //�����邩
			bool is_kinmatic_anglar = true; //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
			bool is_kinmatic_linear = true; //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
			bool is_moveable = true;//������
			bool is_hitable = true;  //�Փ˂��Ȃ�
			bool is_static = false; //static���m�͏Փ˂��� oncoll_enter���������Ȃ� ���ǌy���Ȃ�

			void set_default_physics_data(Physics_data& physics_data) {
				//::: �����l������� :::
				physics_data.inertial_mass      = inertial_mass; //����
				physics_data.drag               = linear_drag; //��C��R
				physics_data.anglar_drag        = anglar_drag; //��C��R
				physics_data.dynamic_friction   = dynamic_friction; //�����C
				physics_data.static_friction    = static_friction; //�Ö��C
				physics_data.restitution        = restitution;	 //�����W��
				physics_data.linear_sleep_threrhold = linear_sleep_threrhold; //freeze��臒l
				physics_data.angula_sleep_threrhold = angula_sleep_threrhold; //freeze��臒l

				physics_data.is_fallable        = is_fallable; //�����Ȃ�
				physics_data.is_kinmatic_anglar = is_kinmatic_anglar;//�e�������Ȃ�(fall�͂���)
				physics_data.is_kinmatic_linear = is_kinmatic_linear;//�e�������Ȃ�(fall�͂���)
				physics_data.is_moveable        = is_moveable; //�����Ȃ�
				physics_data.is_hitable         = is_hitable;  //�Փ˂��Ȃ�
			}
		};

		class Physics_manager
		{
		private:
			static LARGE_INTEGER frame_count; //
			static LARGE_INTEGER frame_count_stop; //
			static LARGE_INTEGER update_start_time; //
			static LARGE_INTEGER update_end_time; //
			static float update_time; //

			static std::mutex mtx; //���add_collder,phsics,joint�� added_data�̈����̎�

			static u_int collider_index_count; //collider��unique�Ȓl������U�邽�ߍ쐬������ۑ�

			// �edata�̎��Ԕz��
			static std::list<Physics_function::ALP_Collider*> ALP_colliders;
			static std::list<Physics_function::ALP_Physics*>  ALP_physicses;
			static std::list<Physics_function::ALP_Collider*> added_buffer_ALP_colliders; //�}���`�X���b�h�p �����̓r���Œǉ����ꂽ�v�f
			static std::list<Physics_function::ALP_Physics*>  added_buffer_ALP_physicses; //�}���`�X���b�h�p �����̓r���Œǉ����ꂽ�v�f
			static std::list<Physics_function::ALP_Collider*> deleted_buffer_ALP_colliders; //�}���`�X���b�h�p �����̓r����GO���폜���ꂽ�v�f
			static std::list<Physics_function::ALP_Physics*>  deleted_buffer_ALP_physicses; //�}���`�X���b�h�p �����̓r����GO���폜���ꂽ�v�f

			static std::list<Physics_function::ALP_Joint*> ALP_joints;
			static std::list<Physics_function::ALP_Joint*> added_buffer_ALP_joints; //�}���`�X���b�h�p �����̓r���Œǉ����ꂽ�v�f
			static std::list<Physics_function::ALP_Joint*> deleted_buffer_ALP_joints; //�}���`�X���b�h�p �����̓r���Œǉ����ꂽ�v�f

			static std::vector<Physics_function::Contacts::Contact_pair*> pairs[2];
			static u_int pairs_new_num; //pairs�̂ǂ������V�����Փ˂Ȃ̂�

			// ���̃t���[���Ԃŕω���������(broad_phase�p)
			static std::vector<Physics_function::ALP_Collider*> moved_collider_for_insertsort; //�}���\�[�g�p ������
			static std::vector<Physics_function::ALP_Collider*> added_collider_for_insertsort; //�}���\�[�g�p �ǉ����ꂽ

			static std::thread physics_thread; //true�ɂȂ����Ƃ�thread_update���~�߂�
			static bool is_stop_physics_thread; //true�ɂȂ����Ƃ�thread_update���~�߂�

		public:
			static volatile bool is_added_ALPcollider;    //Collider���ǉ����ꂽ�Ƃ�true�ɂȂ�

			// ��������physics�̒l
			static Physics_function::PhysicsParams physicsParams;

			static bool is_draw_collider; //COllider�̕\��
			static bool is_draw_dop; //dop�̕\��(14-DOP��\������̂��߂�ǂ���������AABB��\������)
			static bool is_draw_joint; //joint�̕\��


		public:
			struct ColliderPhysics_ptrs {
				ALP_Collider* ALPcollider_ptr = nullptr;
				ALP_Physics* ALPphysics_ptr = nullptr;
			};

			static ColliderPhysics_ptrs add_collider(Collider* coll, void* GO_ptr) {
				std::lock_guard <std::mutex> lock(mtx);

				is_added_ALPcollider = true;

				ColliderPhysics_ptrs ret;
				ALP_Collider*& ALPcollider_ptr = ret.ALPcollider_ptr;
				ALP_Physics*& ALPphysics_ptr = ret.ALPphysics_ptr;

				{
					//itr���ق������ߋ�̃|�C���^�Řg�������
					Physics_function::ALP_Collider* null_coll = nullptr;
					Physics_function::ALP_Physics* null_phy = nullptr;

					added_buffer_ALP_colliders.emplace_back(null_coll);
					added_buffer_ALP_physicses.emplace_back(null_phy);

					auto collider_itr = added_buffer_ALP_colliders.end();
					collider_itr--;
					auto physics_itr = added_buffer_ALP_physicses.end();
					physics_itr--;

					__int64 ID = reinterpret_cast<__int64>(GO_ptr);
					//collider�̃A�h���X�������� (ALPphysics,ALPcollider�̃R���X�g���N�^�ɂ��݂��̃A�h���X���K�v�Ȃ���)
					ALPcollider_ptr = newD Physics_function::ALP_Collider(ID, coll, collider_itr, nullptr, collider_index_count);

					//phsics���g��������
					ALPphysics_ptr = newD Physics_function::ALP_Physics(ID, physics_itr, ALPcollider_ptr, collider_index_count);

					//collider��ALPphysics_ptr�ɒl������
					ALPcollider_ptr->set_ALPphysics_ptr(ALPphysics_ptr);
					//*ALPcollider_ptr = Physics_function::ALP_Collider(coll->gameobject, coll, collider_itr, ALPphysics_ptr, Sce, collider_index_count[Sce]);

					//�C�e���[�^�[��manager�̔z��Ƀ|�C���^��ۑ�
					*collider_itr = ALPcollider_ptr;
					*physics_itr = ALPphysics_ptr;
				}

				//::: �����l������� :::
				physicsParams.set_default_physics_data(coll->physics_data);
				//coll->physics_data.inertial_mass = physicsParams.inertial_mass; //����
				//coll->physics_data.drag = physicsParams.linear_drag; //��C��R
				//coll->physics_data.anglar_drag = physicsParams.anglar_drag; //��C��R
				//coll->physics_data.dynamic_friction = physicsParams.dynamic_friction; //�����C
				//coll->physics_data.static_friction = physicsParams.static_friction; //�Ö��C
				//coll->physics_data.restitution = physicsParams.restitution;	 //�����W��

				//coll->physics_data.is_fallable = physicsParams.is_fallable; //�����Ȃ�
				//coll->physics_data.is_kinmatic_anglar = physicsParams.is_kinmatic_anglar;//�e�������Ȃ�(fall�͂���)
				//coll->physics_data.is_kinmatic_linear = physicsParams.is_kinmatic_linear;//�e�������Ȃ�(fall�͂���)
				//coll->physics_data.is_moveable = physicsParams.is_moveable; //�����Ȃ�
				//coll->physics_data.is_hitable = physicsParams.is_hitable;  //�Փ˂��Ȃ�

				collider_index_count++;

				return ret;
			}

			static ALP_Joint* add_Joint() {
				std::lock_guard <std::mutex> lock(mtx);

				is_added_ALPcollider = true;

				//����ۂ̃|�C���^�Řg�����m��(itr���R���X�g���N�^�Ŏg����������)
				ALP_Joint* null_ptr;
				added_buffer_ALP_joints.emplace_back(null_ptr);
				auto itr = added_buffer_ALP_joints.end();
				itr--;

				//�v�Z�p�̋��ʂ̃N���X�𐶐�
				ALP_Joint* ALPjoint = newD ALP_Joint(itr);
				//���g������
				*itr = ALPjoint;

				ALPjoint->set_this_itr(itr);

				return ALPjoint;
			};


			//::: �}���`�X���b�h���� �폜���镨��buffer�ɂ������񂵂܂� :::
			static void add_delete_buffer_ALPCollider_and_ALPPhsics(Physics_function::ALP_Collider* coll, Physics_function::ALP_Physics* phys) {
				std::lock_guard <std::mutex> lock(mtx);
				deleted_buffer_ALP_colliders.emplace_back(coll);
				coll->is_deleted = true;

				deleted_buffer_ALP_physicses.emplace_back(phys);
			}
			static void add_delete_buffer_ALPJoint(Physics_function::ALP_Joint* joint) {
				std::lock_guard <std::mutex> lock(mtx);
				deleted_buffer_ALP_joints.emplace_back(joint);
			}

			//::: �z�񂩂�̍폜�Bdelete�͂��Ȃ�(physics_manager��loop����ĂԂ���mutex�͕s�v) :::
			static void remove_Joint(std::list<Physics_function::ALP_Joint*>::iterator joint_itr) {
				{
					// joint�̍폜�Ƃ���joint�̂���collider�̍폜�������ɔ��������� delete��2��s���Ď��ʂ���
					// joint�̍폜���ɕۑ������buffer����Ή�����ptr��nullptr�ɂ���
					auto itr = deleted_buffer_ALP_joints.begin();
					auto itr_end = deleted_buffer_ALP_joints.end();
					for (; itr != itr_end; ++itr) {
						if (*itr == *joint_itr) {
							//deleted_buffer_ALP_joints.erase(joint_itr);
							*itr = nullptr;
							//return;
						}
					}
				}

				if ((*joint_itr)->is_added)
					ALP_joints.erase(joint_itr);
				else
					added_buffer_ALP_joints.erase(joint_itr);
			};
			static void remove_ALPcollider(
				std::list<Physics_function::ALP_Collider*>::iterator ALPcoll_itr
			) {
				remove_collider_broad_phase(*ALPcoll_itr);

				if ((*ALPcoll_itr)->is_added) {
					// ���C���̔z��Ɉ����z������ Broadphase���Ă΂�Ă��Ȃ��ꍇadded_collider���|�C���^�������Ă��邽��nullptr�ɂ���
					// �ǉ����āAbroadphase���Ă΂��O��delete�����K�v�����邽�߂�����\�����Ⴂ
					// added�̐�����ɑ����Ȃ�\�����Ⴂ���� ��������ɂ���
					for (auto& coll : added_collider_for_insertsort) if (coll == *ALPcoll_itr)coll = nullptr;
					for (auto& coll : moved_collider_for_insertsort) if (coll == *ALPcoll_itr)coll = nullptr;

					ALP_colliders.erase(ALPcoll_itr);
				}
				else {
					added_buffer_ALP_colliders.erase(ALPcoll_itr);
				}
			}
			static void remove_ALPphysics(
				std::list<Physics_function::ALP_Physics*>::iterator ALPphs_itr
			) {
				if ((*ALPphs_itr)->is_added)
					ALP_physicses.erase(ALPphs_itr);
				else
					added_buffer_ALP_physicses.erase(ALPphs_itr);
			}

		public:
			//::::::
			//raycast���s��
			// ray_pos : ray�̎n�_
			// ray_dir : ray�̌���
			// tag : ray�̏Փ˂���tag
			// ray_min
			// Sce  : Ray�̏Փ˂���collider�̑��݂���scene
			// ---out---
			// tmin : ray�̍ŋߓ_
			// tmax : ray�̍ŉ��_
			// normal : �Փ˂����ʂ�normal
			// coll : �Փ˂���coiider�ւ̃|�C���^
			//:::::::
			static bool ray_cast(
				const Vector3& Ray_pos, const Vector3& Ray_dir,
				u_int tag,
				const float& ray_min,
				float& tmin, float& tmax,
				Vector3& normal,
				Collider*& coll
			);

			//::::::
			//spherecast���s��
			// ray_pos : ray�̎n�_
			// ray_dir : ray�̌���
			// tag : ray�̏Փ˂���tag
			// ray_min
			// Sce  : Ray�̏Փ˂���collider�̑��݂���scene
			// ---out---
			// tmin : ray�̍ŋߓ_
			// tmax : ray�̍ŉ��_
			// normal : �Փ˂����ʂ�normal
			// coll : �Փ˂���coiider�ւ̃|�C���^
			//:::::::
				static bool sphere_cast(
				const Vector3& Ray_pos, const Vector3& Ray_dir,
				const float& radius,
				Vector3& contact_point,
				u_int tag,
				const float& ray_min,
				float& tmin, float& tmax,
				Vector3& normal,
				Collider*& coll
				);

			// ���������̂Ƃ���moved_collider�ɓo�^(sweep&prune�ł̑}���\�[�g�ɂĎg��)
			static void add_moved(Physics_function::ALP_Collider* coll) {
				moved_collider_for_insertsort.push_back(coll);
			}

		private:
			// �X���b�h�p update while����update���񂵂Ă���
			static void thread_update();

			// �ǉ����ꂽ���̂�K������(�}���`�X���b�h���Ə����r���ɒǉ�����邽��buffer������)
			static void adapt_added_data(bool is_mutex_lock = true);

			// �폜���ꂽ���̂�K������(�}���`�X���b�h���Ə����r����GO���폜����邽��buffer������)
			static void dadapt_delete_data(bool is_mutex_lock = true);

		public:
			//// gameobject.transform��ALPcollider.transform�ōX�V���� gameobject��transform���ω�������true��Ԃ�
			//static bool adapt_transform_to_gameobject();

			//// gameobject��transform���R�s�[����
			//static void copy_gameobject_transform();

			// ��thread��update����
			static void thread_start();

			// ��thread�ł�update���~�߂āAjoin���s��
			static void thread_stop_and_join();

			static void mutex_lock() { mtx.lock(); };
			static void mutex_unlock() { mtx.unlock(); };

			static void timer_stop();
			static void timer_start();

		public:
			// main thread����Ă΂��

			// ���t���[���Ă΂��X�V����
			static bool update();

			// Gui�̕\��
			static bool update_Gui();

			// collider�̕\��
			static bool render_collider();
			// kdop�̕\��
			static bool render_dop();
			// joint�̕\��
			static bool render_joint();

			static void destroy();

		};

	}
}