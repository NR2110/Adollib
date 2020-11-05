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

	namespace physics_function {
		// physics_manager��static�ϐ��̏����l
		class physics_manager_default {
		public:
			static constexpr float gravity = 9.8f; //�d��

			//static physics_function::ALP_Physics default_physics; //�����ݒ肵�Ă��Ȃ��Ƃ���pyisics�̒l cpp�Őݒ�

			static constexpr float bounce_threshold = 0.01f; //���˕Ԃ��臒l
			static constexpr float sleep_threshold = 0.01f; //sleep��臒l

			static constexpr float contact_max_per_pair = 4; //���pair�ŏՓ˂̉��U���s���ő�̐�
			static constexpr float contact_allowable_error = 0.02f; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
			static constexpr float contact_threrhold_normal = 0.004f; //�Փ˓_��臒l
			static constexpr float contact_threrhold_tangent = 0.02f;//�Փ˓_��臒l

			static constexpr float bias = 0.15f;//�߂荞�݂𒼂���
			static constexpr float slop = 0.003f;//�Փ˂̋��e�l

			static constexpr int solver_iterations = 5; //�Փ˂̐��x
			static constexpr bool hit_backfaces_flag = false;//mesh�̌�납��Փ˂��邩

			static constexpr float timeStep = 0.016f;
		};
	}

	class Phyisics_manager
	{
	private:
		static int frame_count;

		//collider_component�̃|�C���^�z��
		static std::unordered_map<Scenelist, std::list<Collider*>> colliders;

		//�edata�̎��Ԕz��
		static std::unordered_map<Scenelist, std::list<physics_function::ALP_Collider>> ALP_colliders;
		static std::unordered_map<Scenelist, std::list<physics_function::ALP_Physics>> ALP_physicses;

		static std::vector<physics_function::Contacts::Contact_pair> pairs;
		static std::vector<physics_function::Contacts::Collider_2> broad_mid_pair;

		static bool is_changed_coll; //�V����Collider���ǉ����ꂽ�A�폜���ꂽ�ꍇtrue
	public:
		static bool is_changed_colliders() { return is_changed_colliders; };

	public:
		static float gravity; //�d��

		static physics_function::ALP_Physics default_physics; //�����ݒ肵�Ă��Ȃ��Ƃ���pyisics�̒l

		static float bounce_threshold; //���˕Ԃ��臒l
		static float sleep_threshold; //sleep��臒l

		static float contact_max_per_pair; //���pair�ŏՓ˂̉��U���s���ő�̐�
		static float contact_allowable_error; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
		static float contact_threrhold_normal; //�Փ˓_��臒l
		static float contact_threrhold_tangent;//�Փ˓_��臒l

		static float bias;//�߂荞�݂𒼂���
		static float slop;//�Փ˂̋��e�l

		static int solver_iterations; //�Փ˂̐��x
		static bool hit_backfaces_flag;//mesh�̌�납��Փ˂��邩

		static float timeStep; 

	public:

		static physics_function::ColliderPhysicsShape_itrs add_collider(Collider* coll, Scenelist Sce = Scene::now_scene) {

			physics_function::ColliderPhysicsShape_itrs ret;
			{
				colliders[Sce].emplace_back(coll);
				ret.coll_itr = colliders[Sce].end();
				ret.coll_itr--;
			}
			{
				physics_function::ALP_Collider ALPcollider;
				ALP_colliders[Sce].emplace_back(ALPcollider);
				ret.ALPcollider_itr = ALP_colliders[Sce].end();
				ret.ALPcollider_itr--;
			}
			{
				physics_function::ALP_Physics ALPphysics;
				ALP_physicses[Sce].emplace_back(ALPphysics);
				ret.ALPphysics_itr = ALP_physicses[Sce].end();
				ret.ALPphysics_itr--;
			}

			return ret;
		}

		static void remove_collider(Adollib::Collider* R, Scenelist Sce = Scene::now_scene) {
			ALP_colliders[Sce].erase(R->get_itrs().ALPcollider_itr);
			ALP_physicses[Sce].erase(R->get_itrs().ALPphysics_itr);

			colliders[Sce].erase(R->get_itrs().coll_itr);
		}

	public:

		// ���t���[���Ă΂��X�V����
		static bool update(Scenelist Sce = Scene::now_scene);

		static void destroy(Scenelist Sce = Scene::now_scene);

	};

}