#include "ALP__physics_function.h"
#include "ALP__physics_manager.h"
#include "../Scene/scene.h"
#include "../Main/Adollib.h"

#include "../Object/gameobject_manager.h"
#include "../Imgui/work_meter.h"

#include "collider.h"

using namespace Adollib;
using namespace physics_function;
using namespace Contacts;

//::: staticメンバの初期化 :::::
namespace Adollib
{
	float Phyisics_manager::gravity = Physics_manager_default::gravity; //重力

	physics_function::ALP_Physics Phyisics_manager::default_physics = ALP_Physics(
		Physics_manager_default::inertial_mass, //質量
		Physics_manager_default::drag, //空気抵抗
		Physics_manager_default::anglar_drag, //空気抵抗
		Physics_manager_default::dynamic_friction,//動摩擦
		Physics_manager_default::static_friction, //静摩擦
		Physics_manager_default::restitution,	 //反発係数

		Physics_manager_default::is_fallable, //落ちない
		Physics_manager_default::is_kinematic, //影響うけない(fallはする)
		Physics_manager_default::is_moveable, //動かない
		Physics_manager_default::is_hitable //衝突しない
	); //何も設定していないときのpyisicsの値

	float Phyisics_manager::bounce_threshold = Physics_manager_default::bounce_threshold; //跳ね返りの閾値
	float Phyisics_manager::sleep_threshold = Physics_manager_default::sleep_threshold; //sleepの閾値

	float Phyisics_manager::contact_allowable_error = Physics_manager_default::contact_allowable_error; //これ以上近いと同一衝突点とする
	float Phyisics_manager::contact_threrhold_normal = Physics_manager_default::contact_threrhold_normal; //衝突点の閾値
	float Phyisics_manager::contact_threrhold_tangent = Physics_manager_default::contact_threrhold_tangent;//衝突点の閾値

	float Phyisics_manager::bias = Physics_manager_default::bias;//めり込みを直す力
	float Phyisics_manager::slop = Physics_manager_default::slop;//衝突の許容値

	int Phyisics_manager::solver_iterations = Physics_manager_default::solver_iterations; //衝突の精度
	bool Phyisics_manager::hit_backfaces_flag = Physics_manager_default::hit_backfaces_flag;//meshの後ろから衝突するか

	float Phyisics_manager::timeStep = Physics_manager_default::timeStep;
}

namespace Adollib
{
	int Phyisics_manager::frame_count;

	//collider_componentのポインタ配列
	std::unordered_map<Scenelist, std::list<Collider*>> Phyisics_manager::colliders;

	//各dataの実態配列
	std::unordered_map<Scenelist, std::list<physics_function::ALP_Collider>> Phyisics_manager::ALP_colliders;
	std::unordered_map<Scenelist, std::list<physics_function::ALP_Physics>> Phyisics_manager::ALP_physicses;

	std::vector<physics_function::Contacts::Contact_pair> Phyisics_manager::pairs;
	std::vector<physics_function::Contacts::Collider_2> Phyisics_manager::broad_mid_pair;

	bool Phyisics_manager::is_changed_coll = false; //新たにColliderが追加された、削除された場合true
}

namespace Adollib
{

	bool Phyisics_manager::update(Scenelist Sce)
	{

		if (frame_count < 10) {
			resetforce(ALP_physicses[Sce]);
			frame_count++;
			return true;
		}


		Work_meter::start("refresh_ALP_from_data");
		// get_dataのdataをColliderに入力 
		refresh_ALP_from_data(ALP_colliders[Sce]);	
		Work_meter::stop("refresh_ALP_from_data");


		// ColliderのWorld情報の更新
		update_world_trans(ALP_colliders[Sce]);

		// 外力の更新
		applyexternalforce(ALP_physicses[Sce]);

		timeStep = Al_Global::second_per_frame;

		// 大雑把な当たり判定
		Work_meter::start("Broad,Mid,Narrow");

		Work_meter::start("Broadphase");
		Work_meter::tag_start("Broadphase");
		Broadphase(ALP_physicses[Sce], broad_mid_pair);
		is_changed_coll = false;
		Work_meter::tag_stop();
		Work_meter::stop("Broadphase");

		Work_meter::start("Midphase");
		Work_meter::tag_start("Midphase");
		Midphase(broad_mid_pair, pairs);
		Work_meter::tag_stop();
		Work_meter::stop("Midphase");

		// 衝突生成
		Work_meter::start("Narrowphase");
		Work_meter::tag_start("Narrowphase");
		generate_contact(pairs);
		Work_meter::tag_stop();
		Work_meter::stop("Narrowphase");

		Work_meter::stop("Broad,Mid,Narrow");

		// 衝突解決
		Work_meter::start("Resolve");
		Work_meter::tag_start("Resolve");
		resolve_contact(ALP_colliders[Sce], pairs);
		Work_meter::tag_stop();
		Work_meter::stop("Resolve");

		// 位置の更新
		integrate(ALP_physicses[Sce]);

		// GOへColliderの影響を渡す
		solv_resolve(ALP_colliders[Sce]);
		resolve_gameobject(ALP_colliders[Sce]);

		return true;

	}


	void Phyisics_manager::destroy(Scenelist Sce) {
		colliders[Sce].clear();
	}
}


namespace Adollib {






}