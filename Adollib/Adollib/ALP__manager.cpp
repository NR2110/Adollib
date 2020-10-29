
#include "ALP__physics_function.h"
#include "ALP__manager.h"
#include "scene.h"
#include "Adollib.h"


using namespace Adollib;
using namespace Contacts;
using namespace physics_function;

#include "gameobject_manager.h"
#include "work_meter.h"

namespace Adollib
{

	int Rigitbody_manager::flame_count = 0;

	std::unordered_map<Scenelist, std::list<Collider*>>   Rigitbody_manager::colliders;

	std::vector<Contacts::Contact_pair> Rigitbody_manager::pairs;

	bool Rigitbody_manager::update(Scenelist Sce)
	{
		std::vector<Contact> contacts;
		float resituation = Al_Global::base_resituation;

		std::list<Adollib::Collider*>::iterator collitr = Rigitbody_manager::colliders[Sce].begin();
		std::list<Adollib::Collider*>::iterator collitr_end = Rigitbody_manager::colliders[Sce].end();


		//world空間での情報を更新
		for (collitr = Rigitbody_manager::colliders[Sce].begin();collitr != collitr_end; collitr++) {
			(*collitr)->update_world_trans();
			(*collitr)->onColl = false;
		}

		// 外力の更新
		applyexternalforce(Rigitbody_manager::colliders[Sce]);

		physics_g::timeStep = Al_Global::second_per_frame;

		static std::vector<Contacts::Collider_2> broad_mid_pair;

		// 大雑把な当たり判定
		Work_meter::start("Broad,Mid,Narrow");
		Work_meter::start("Broadphase");
		Work_meter::tag_start("Broadphase");
		Broadphase(Rigitbody_manager::colliders[Sce], broad_mid_pair, pairs);
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
		resolve_contact(Rigitbody_manager::colliders[Sce], pairs);
		Work_meter::tag_stop();
		Work_meter::stop("Resolve");

		//位置の更新
		integrate(Rigitbody_manager::colliders[Sce]);

		for (collitr = Rigitbody_manager::colliders[Sce].begin(); collitr != collitr_end; collitr++) {
			(*collitr)->solv_resolve();
		}
		for (collitr = Rigitbody_manager::colliders[Sce].begin(); collitr != collitr_end; collitr++) {
			(*collitr)->resolve_gameobject();
		}

		return true;

	}


		void Rigitbody_manager::destroy(Scenelist Sce) {
			colliders[Sce].clear();
		}
	}


namespace Adollib {

	//void Rigitbody_manager::remove_collider(Adollib::Collider* R, Scenelist Sce) {

	//	std::list<std::shared_ptr<Adollib::Collider>>::iterator collitr = Rigitbody_manager::colliders[Sce].begin();
	//	std::list<std::shared_ptr<Adollib::Collider>>::iterator collitr_end = Rigitbody_manager::colliders[Sce].end();

	//	for (collitr = Rigitbody_manager::colliders[Sce].begin(); collitr != collitr_end; collitr++) {
	//		if ((*collitr) == R) {
	//			Rigitbody_manager::colliders[Sce].erase(collitr);
	//			return;
	//		}
	//	}
	//}

	void Rigitbody_manager::remove_collider_perGO(Adollib::Gameobject* GO, Scenelist Sce) {

		//std::list<Adollib::Collider*>::iterator collitr = Rigitbody_manager::colliders[Sce].begin();
		//std::list<Adollib::Collider*>::iterator collitr_end = Rigitbody_manager::colliders[Sce].end();
		//int gocolsize = GO->collider.size();
		//for (int i = 0; i < gocolsize; i++) {
		//	for (collitr = Rigitbody_manager::colliders[Sce].begin(); collitr != collitr_end; collitr++) {
		//		if (*collitr == GO->collider[i].) {
		//			Rigitbody_manager::colliders[Sce].erase(collitr);
		//			return;
		//		}
		//	}
		//}
	}






}