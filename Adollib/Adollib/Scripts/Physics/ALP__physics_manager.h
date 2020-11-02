#pragma once
#include "../Object/component.h"
#include "collider.h"
#include "../Scene/scene_list.h"
#include "../Scene/scene.h"

#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_shape.h"

#include <map>

#include "contact.h"

namespace Adollib
{

	class Phyisics_manager
	{
	private:
		static int flame_count;

		static std::vector<Adollib::physics_function::ALP_Collider> ALP_colliders;
		static std::vector<Adollib::physics_function::ALP_Physics> ALP_physicses;
		static std::vector<Adollib::physics_function::Shape> ALP_shapes;


		static std::vector<Contacts::Contact_pair> pairs;

	public:
		static float gravity; //�d��

		static ALP_Physics default_physics; //�����ݒ肵�Ă��Ȃ��Ƃ���pyisics�̒l

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





		//No_hit_tag��"all"������Ƃ��ׂĂ̂��̂ɏՓ˂��Ȃ��Ȃ��
		static std::list<Adollib::Collider*>::iterator add_collider(Collider* coll, Scenelist Sce = Scene::now_scene) {

			colliders[Sce].emplace_back(coll);

			auto ret = colliders[Sce].end();
			ret--;

			(*ret)->coll_itr = ret;

			return ret;
		}

		static void remove_collider(std::list<Adollib::Collider*>::iterator itr, Scenelist Sce = Scene::now_scene) {
			colliders[Sce].erase(itr);
		}

		static void remove_collider(Adollib::Collider* R, Scenelist Sce = Scene::now_scene) {
			colliders[Sce].erase(R->coll_itr);
		}
		static void remove_collider_perGO(Adollib::Gameobject* GO, Scenelist Sce = Scene::now_scene);

	public:

		// ���t���[���Ă΂��X�V����
		static bool update(Scenelist Sce = Scene::now_scene);

		static void destroy(Scenelist Sce = Scene::now_scene);

	};

}