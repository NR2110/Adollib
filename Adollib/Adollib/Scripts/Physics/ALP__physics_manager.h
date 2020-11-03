#pragma once
#include <map>
#include "../Object/component.h"
#include "../Scene/scene.h"

//#include "contact.h"
#include "collider__base.h"
#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_shape.h"


namespace Adollib
{
	class Collider;

	class Phyisics_manager
	{
	private:
		static int frame_count;

		//collider_component�̃|�C���^�z��
		static std::unordered_map<Scenelist, std::list<Collider*>> colliders;

		//�edata�̎��Ԕz��
		static std::unordered_map<Scenelist, std::list<physics_function::ALP_Collider>> ALP_colliders;
		static std::unordered_map<Scenelist, std::list<physics_function::ALP_Physics>> ALP_physicses;
		static std::unordered_map<Scenelist, std::list<physics_function::ALP_Shape>> ALP_shapes;



		static std::vector<Contacts::Contact_pair> pairs;

	public:
		static float gravity; //�d��

		static physics_function::ALP_Physics default_physics; //�����ݒ肵�Ă��Ȃ��Ƃ���pyisics�̒l

		static float bounce_threshold; //���˕Ԃ��臒l
		static float sleep_threshold; //sleep��臒l

		static float contact_max_per_pair; //���pair�ŏՓ˂̉��U���s���ő�̐�
		static float contact_allowable_error; //����ȏ�߂��Ɠ���Փ˓_�Ƃ���
		static float contact_threrhold_normal; //�Փ˓_��臒l
		static float contact_threrhold_tangent;//�Փ˓_��臒l
		static float contact_pairs_mode; //�ڐG��pair�̐����������Ȃ��^�C�v (kinematic�ǂ�����pair�𐶐������肵�Ȃ�������)

		static float bias;//�߂荞�݂𒼂���
		static float slop;//�Փ˂̋��e�l

		static float solver_iterations; //�Փ˂̐��x
		static float hit_backfaces_flag;//mesh�̌�납��Փ˂��邩



	public:

		//No_hit_tag��"all"������Ƃ��ׂĂ̂��̂ɏՓ˂��Ȃ��Ȃ��
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
			{
				physics_function::ALP_Shape ALPshape;
				ALP_shapes[Sce].emplace_back(ALPshape);
				ret.ALPshape_itr = ALP_shapes[Sce].end();
				ret.ALPshape_itr--;
			}

			return ret;
		}

		static void remove_collider(Adollib::Collider* R, Scenelist Sce = Scene::now_scene) {
			ALP_colliders[Sce].erase(R->get_itrs().ALPcollider_itr);
			ALP_physicses[Sce].erase(R->get_itrs().ALPphysics_itr);
			ALP_shapes[Sce].erase(R->get_itrs().ALPshape_itr);

			colliders[Sce].erase(R->get_itrs().coll_itr);
		}

	public:

		// ���t���[���Ă΂��X�V����
		static bool update(Scenelist Sce = Scene::now_scene);

		static void destroy(Scenelist Sce = Scene::now_scene);

	};

}