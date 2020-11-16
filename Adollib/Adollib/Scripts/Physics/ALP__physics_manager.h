#pragma once

#include <map>
#include "../Object/component.h"
#include "../Scene/scene.h"

#include "collider__base.h"
#include "ALP_collider.h"
#include "ALP_physics.h"

#include "contact.h"

#include "ALP__shapes.h"

namespace Adollib
{
	class Collider;

	namespace Physics_function {
		// physics_manager��static�ϐ��̏����l
		class Physics_manager_default {
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
			static constexpr float drag = 0.1f; //��C��R
			static constexpr float anglar_drag = 0.1f; //��C��R
			static constexpr float dynamic_friction = 0.4f;//�����C
			static constexpr float static_friction = 0.4f; //�Ö��C
			static constexpr float restitution = 0.1f;	 //�����W��

			static constexpr bool is_fallable = true; //�����邩
			static constexpr bool is_kinematic = true; //�e�������邩(fall�͂���)
			static constexpr bool is_moveable = true;//������
			static constexpr bool is_hitable = true;  //�Փ˂��Ȃ�

			//::::::::
		};
	}

	class Phyisics_manager
	{
	private:
		static int frame_count;

		//collider_component�̃|�C���^�z��
		static std::unordered_map<Scenelist, std::list<Collider*>> colliders;

		//�edata�̎��Ԕz��
		static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider>> ALP_colliders;
		static std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics>> ALP_physicses;

		static std::vector<Physics_function::Contacts::Contact_pair> pairs;
		static std::vector<Physics_function::Contacts::Collider_2> broad_mid_pair;

		static bool is_changed_coll; //�V����Collider���ǉ����ꂽ�A�폜���ꂽ�ꍇtrue
	public:
		static bool is_changed_colliders() { return is_changed_coll; };

	public:
		static float gravity; //�d��

		static Physics_function::ALP_Physics default_physics; //�����ݒ肵�Ă��Ȃ��Ƃ���pyisics�̒l

		static float bounce_threshold; //���˕Ԃ��臒l
		static float sleep_threshold; //sleep��臒l

		static float contact_allowable_error; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
		static float contact_threrhold_normal; //�Փ˓_��臒l
		static float contact_threrhold_tangent;//�Փ˓_��臒l

		static float bias;//�߂荞�݂𒼂���
		static float slop;//�Փ˂̋��e�l

		static int solver_iterations; //�Փ˂̐��x
		static bool hit_backfaces_flag;//mesh�̌�납��Փ˂��邩

		static float timeStep; 

		static bool is_draw_collider; //COllider�̕\��
		static bool is_draw_dop; //dop�̕\��(14-DOP��\������̂��߂�ǂ���������AABB��\������)

	public:

		static Physics_function::ColliderPhysics_itrs add_collider(Collider* coll, Scenelist Sce = Scene::now_scene) {

			is_changed_coll = true;

			Physics_function::ColliderPhysics_itrs ret;
			{
				colliders[Sce].emplace_back(coll);
				ret.coll_itr = colliders[Sce].end();
				ret.coll_itr--;
			}
			{
				Physics_function::ALP_Collider ALPcollider;

				ALP_colliders[Sce].emplace_back(ALPcollider);
				ret.ALPcollider_itr = ALP_colliders[Sce].end();
				ret.ALPcollider_itr--;
			}
			{
				Physics_function::ALP_Physics ALPphysics;
				ALPphysics = Phyisics_manager::default_physics;

				ALP_physicses[Sce].emplace_back(ALPphysics);
				ret.ALPphysics_itr = ALP_physicses[Sce].end();
				ret.ALPphysics_itr--;
			}

			ret.ALPcollider_itr->ALPphysics = ret.ALPphysics_itr;
			ret.ALPphysics_itr->ALP_coll = ret.ALPcollider_itr;

			//::: �����l������� :::
			(*ret.coll_itr)->inertial_mass = Phyisics_manager::default_physics.inertial_mass; //����
			(*ret.coll_itr)->drag = Phyisics_manager::default_physics.drag; //��C��R
			(*ret.coll_itr)->anglar_drag = Phyisics_manager::default_physics.anglar_drag; //��C��R
			(*ret.coll_itr)->dynamic_friction = Phyisics_manager::default_physics.dynamic_friction; //�����C
			(*ret.coll_itr)->static_friction = Phyisics_manager::default_physics.static_friction; //�Ö��C
			(*ret.coll_itr)->restitution = Phyisics_manager::default_physics.restitution;	 //�����W��

			(*ret.coll_itr)->is_fallable = Phyisics_manager::default_physics.is_fallable; //�����Ȃ�
			(*ret.coll_itr)->is_kinematic = Phyisics_manager::default_physics.is_kinematic;//�e�������Ȃ�(fall�͂���)
			(*ret.coll_itr)->is_moveable = Phyisics_manager::default_physics.is_moveable; //�����Ȃ�
			(*ret.coll_itr)->is_hitable = Phyisics_manager::default_physics.is_hitable;  //�Փ˂��Ȃ�

			return ret;
		}

		static void remove_collider(
			std::list<Collider*>::iterator coll_itr, 
			std::list<Physics_function::ALP_Collider>::iterator ALPcoll_itr, 
			std::list<Physics_function::ALP_Physics>::iterator ALPphys_itr, 
			Scenelist Sce = Scene::now_scene) {
			is_changed_coll = true;
			ALP_colliders[Sce].erase(ALPcoll_itr);
			ALP_physicses[Sce].erase(ALPphys_itr);

			colliders[Sce].erase(coll_itr);
		}

		static void remove_collider(
			std::list<Collider*>::iterator coll_itr,
			std::list<Physics_function::ALP_Collider>::iterator ALPcoll_itr,
			Scenelist Sce = Scene::now_scene) {
			is_changed_coll = true;
			ALP_colliders[Sce].erase(ALPcoll_itr);

			colliders[Sce].erase(coll_itr);
		}

		public:
			static bool ray_cast(
				const Vector3& Ray_pos, const Vector3& Ray_dir,
				u_int tag,
				float& tmin, float& tmax,
				Vector3& normal,
				Collider* coll,
				Scenelist Sce = Scene::now_scene);

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