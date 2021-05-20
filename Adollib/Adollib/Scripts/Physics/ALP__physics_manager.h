#pragma once

#include <map>
#include "../Object/component.h"
#include "../Object/gameobject.h"
#include "../Scene/scene.h"

#include "collider.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

#include "ALP_broadphase.h"

#include "contact.h"

#include "ALP__shapes.h"

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

			float contact_allowable_error = 0.02f; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
			float contact_threrhold_normal = 0.004f; //�Փ˓_��臒l
			float contact_threrhold_tangent = 0.02f;//�Փ˓_��臒l



			float bias = 0.15f;//�߂荞�݂𒼂���
			float slop = 0.003f;//�Փ˂̋��e�l

			int solver_iterations = 5; //�Փ˂̐��x
			bool hit_backfaces_flag = false;//mesh�̌�납��Փ˂��邩

			float timeStep = 0.016f;

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
	}

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

		static std::vector<Physics_function::Contacts::Contact_pair> pairs;
		static std::vector<Physics_function::Contacts::Collider_2> broad_mid_pair;

		//���̃t���[���Ԃŕω���������(broad_phase�p)
		static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> moved_collider;   //������
		static std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> added_collider;   //�ǉ����ꂽ

	public:
		//��������physics�̒l
		static Physics_function::PhysicsParams physicsParams;

		//static float gravity; //�d��

		//static float bounce_threshold; //���˕Ԃ��臒l
		//static float sleep_threshold; //sleep��臒l

		//static float contact_allowable_error; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
		//static float contact_threrhold_normal; //�Փ˓_��臒l
		//static float contact_threrhold_tangent;//�Փ˓_��臒l

		//static float bias;//�߂荞�݂𒼂���
		//static float slop;//�Փ˂̋��e�l

		//static int solver_iterations; //�Փ˂̐��x
		//static bool hit_backfaces_flag;//mesh�̌�납��Փ˂��邩

		//static float timeStep;

		//static float inertial_mass; //����
		//static float linear_drag; //��C��R
		//static float anglar_drag; //��C��R
		//static float dynamic_friction;//�����C
		//static float static_friction; //�Ö��C
		//static float restitution;	 //�����W��

		//static bool is_fallable; //�����邩
		//static bool is_kinmatic_anglar; //�ق��̕��̂���̉e���ŉ�]���x���ω����Ȃ�
		//static bool is_kinmatic_linear; //�ق��̕��̂���̉e���ŕ��i���x���ω����Ȃ�
		//static bool is_moveable;//������
		//static bool is_hitable;  //�Փ˂��Ȃ�


		static bool is_draw_collider; //COllider�̕\��
		static bool is_draw_dop; //dop�̕\��(14-DOP��\������̂��߂�ǂ���������AABB��\������)

	public:

		static Physics_function::ColliderPhysics_ptrs add_collider(Collider* coll) {

			Scenelist Sce = coll->gameobject->get_scene();

			Physics_function::ColliderPhysics_ptrs ret;
			{
				colliders[Sce].emplace_back(coll);
				ret.coll_itr = colliders[Sce].end();
				ret.coll_itr--;
			}
			{
				//itr���ق������ߋ�̃|�C���^�Řg�������
				Physics_function::ALP_Collider* null = nullptr;
				ALP_colliders[Sce].emplace_back(null);

				auto itr = ALP_colliders[Sce].end();
				itr--;

				//���g��������
				*itr = D_new Physics_function::ALP_Collider(coll->gameobject, itr, Sce, collider_index_count[Sce]);

				ret.ALPcollider_ptr = *itr;
			}

			{
				//itr���ق������ߋ�̃|�C���^�Řg�������
				Physics_function::ALP_Physics* null = nullptr;
				ALP_physicses[Sce].emplace_back(null);

				auto itr = ALP_physicses[Sce].end();
				itr--;

				//���g��������
				*itr = D_new Physics_function::ALP_Physics(coll->gameobject, itr, Sce, collider_index_count[Sce]);

				//phsics�̏����l�̓���
				(*itr)->set_default();

				ret.ALPphysics_ptr = *itr;
			}

			ret.ALPcollider_ptr->ALPphysics = ret.ALPphysics_ptr;
			ret.ALPphysics_ptr->ALPcollider = ret.ALPcollider_ptr;

			//�ǉ����ꂽcollider
			added_collider[Sce].emplace_back(ret.ALPcollider_ptr);

			//::: �����l������� :::
			(*ret.coll_itr)->physics_data.inertial_mass =	physicsParams.inertial_mass; //����
			(*ret.coll_itr)->physics_data.drag =			physicsParams.linear_drag; //��C��R
			(*ret.coll_itr)->physics_data.anglar_drag =		physicsParams.anglar_drag; //��C��R
			(*ret.coll_itr)->physics_data.dynamic_friction = physicsParams.dynamic_friction; //�����C
			(*ret.coll_itr)->physics_data.static_friction = physicsParams.static_friction; //�Ö��C
			(*ret.coll_itr)->physics_data.restitution =		physicsParams.restitution;	 //�����W��

			(*ret.coll_itr)->physics_data.is_fallable =		physicsParams.is_fallable; //�����Ȃ�
			(*ret.coll_itr)->physics_data.is_kinmatic_anglar = physicsParams.is_kinmatic_anglar;//�e�������Ȃ�(fall�͂���)
			(*ret.coll_itr)->physics_data.is_kinmatic_linear = physicsParams.is_kinmatic_linear;//�e�������Ȃ�(fall�͂���)
			(*ret.coll_itr)->physics_data.is_moveable =		physicsParams.is_moveable; //�����Ȃ�
			(*ret.coll_itr)->physics_data.is_hitable =		physicsParams.is_hitable;  //�Փ˂��Ȃ�

			collider_index_count[Sce]++;
			return ret;
		}

		//static void remove_collider(
		//	std::list<Collider*>::iterator coll_itr,
		//	Physics_function::ALP_Collider* ALPcoll_itr,
		//	Physics_function::ALP_Physics* ALPphys_itr,
		//	Scenelist Sce = Scene::now_scene);


		//static void remove_collider(
		//	std::list<Collider*>::iterator coll_itr,
		//	Physics_function::ALP_Collider* ALPcoll_itr,
		//	Scenelist Sce = Scene::now_scene);

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