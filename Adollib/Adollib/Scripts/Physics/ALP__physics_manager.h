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
		static float gravity; //重力

		static ALP_Physics default_physics; //何も設定していないときのpyisicsの値

		static float bounce_threshold; //跳ね返りの閾値
		static float sleep_threshold; //sleepの閾値

		static float contact_max_per_pair; //一つのpairで衝突の解散を行う最大の数
		static float contact_allowable_error; //これ以上近いと同一衝突点とする
		static float contact_threrhold_normal; //衝突点の閾値
		static float contact_threrhold_tangent;//衝突点の閾値
		static float contact_pairs_mode; //接触のpairの生成をおこなうタイプ (kinematicどおしでpairを生成したりしなかったり)

		static float bias;//めり込みを直す力
		static float slop;//衝突の許容値

		static float solver_iterations; //衝突の精度
		static float hit_backfaces_flag;//meshの後ろから衝突するか





		//No_hit_tagに"all"があるとすべてのものに衝突しなくなるよ
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

		// 毎フレーム呼ばれる更新処理
		static bool update(Scenelist Sce = Scene::now_scene);

		static void destroy(Scenelist Sce = Scene::now_scene);

	};

}