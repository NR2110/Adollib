#include "ALP__physics_function.h"
#include "ALP__physics_manager.h"
#include "../Scene/scene.h"
#include "../Main/Adollib.h"

#include "../Object/gameobject_manager.h"
#include "../Imgui/imgui_all.h"
#include "../Imgui/work_meter.h"
#include "../Imgui/debug.h"

#include "collider.h"
#include "ALP_joint.h"

#include "ALP_raycast.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

//ContactPointの表示
#define Update60fps
//#define Draw_Contact
//#define Draw_JointContact

//::: staticメンバの初期化 :::::
#pragma region static_initialize

namespace Adollib
{
	volatile bool Physics_manager::is_updated_mainthread = true; //physicsを更新したframeだけtrueになる
	volatile bool Physics_manager::is_updated_physicsthread = true; //physicsを更新したframeだけtrueになる

	int Physics_manager::count_mainthread = 0;   //デバッグ用 main,physicsのupdateの割合を求める
	int Physics_manager::count_physicsthread = 0;//デバッグ用 main,physicsのupdateの割合を求める

	std::thread Physics_manager::physics_thread; //physicsのthread用
	bool Physics_manager::is_stop_physics_thread = false; //trueになったときthread_updateを止める

	LARGE_INTEGER Physics_manager::frame_count;

	std::mutex Physics_manager::mtx;

	// 各dataの実態配列
	std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider*>> Physics_manager::ALP_colliders;
	std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics*>>  Physics_manager::ALP_physicses;
	std::map<Scenelist, std::list<Physics_function::ALP_Collider*>> Physics_manager::added_buffer_ALP_colliders;
	std::map<Scenelist, std::list<Physics_function::ALP_Physics*>>  Physics_manager::added_buffer_ALP_physicses;
	std::map<Scenelist, std::list<Physics_function::ALP_Collider*>> Physics_manager::dalated_buffer_ALP_colliders; //マルチスレッド用 処理の途中でGOが削除された要素
	std::map<Scenelist, std::list<Physics_function::ALP_Physics*>>  Physics_manager::dalated_buffer_ALP_physicses; //マルチスレッド用 処理の途中でGOが削除された要素

	std::list<Physics_function::ALP_Joint*> Physics_manager::ALP_joints;
	std::list<Physics_function::ALP_Joint*> Physics_manager::added_buffer_ALP_joints;
	std::list<Physics_function::ALP_Joint*> Physics_manager::dalated_buffer_ALP_joints;

	std::vector<Physics_function::Contacts::Contact_pair*> Physics_manager::pairs[2];
	u_int Physics_manager::pairs_new_num = 0; //pairsのどっちが新しい衝突なのか
	// std::vector<Physics_function::Contacts::Collider_2> Phyisics_manager::broad_mid_pair;

	std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> Physics_manager::moved_collider_for_insertsort;   //挿入ソート用 動いた
	std::unordered_map<Scenelist, std::vector<Physics_function::ALP_Collider*>> Physics_manager::added_collider_for_insertsort;   //挿入ソート用 追加された

	std::unordered_map<Scenelist, u_int> Physics_manager::collider_index_count;

	Physics_function::PhysicsParams Physics_manager::physicsParams;

	bool Physics_manager::is_draw_collider = false;
	bool Physics_manager::is_draw_dop = false;
	bool Physics_manager::is_draw_joint = false;
}

#pragma endregion

bool Physics_manager::update(Scenelist Sce)
{
	Work_meter::start("Phyisics_manager", 1);

	LARGE_INTEGER time;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
	LONGLONG counts_per_sec;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));
	float seconds_per_count = 1.0f / static_cast<double>(counts_per_sec);

	if (Al_Global::second_per_game < 1) {
		adapt_added_data(Sce);
		reset_data_per_frame(ALP_colliders[Sce], ALP_physicses[Sce]);

		resetforce(ALP_physicses[Sce]);
		is_updated_physicsthread = true;
		is_updated_mainthread = false;
		return true;
	}


	// データの受け渡し部分
	{
		std::lock_guard <std::mutex> lock(mtx);

		// 削除したものを配列から消す
		dadapt_delete_data(false);

		// mainthreadが更新したら
		if (is_updated_mainthread) {

			is_updated_mainthread = false; // mainthreadがgameobject_transformを呼んだあと gameobjectのtransformをphysicsにコピーする

			// 追加したものを配列に加える (is_updated_mainthreadがtrueでないとworld情報が更新されていない可能性がある)
			adapt_added_data(Sce, false);

			// Colliderのframe毎に保存するdataをreset
			reset_data_per_frame(ALP_colliders[Sce], ALP_physicses[Sce]);

		}

		// ColliderのWorld情報の更新
		adapt_component_data(ALP_colliders[Sce], ALP_physicses[Sce], ALP_joints);

	}

	physicsParams.timeStep = ALmin((float)(time.QuadPart - frame_count.QuadPart) * seconds_per_count, physicsParams.max_timeStep);
	Work_meter::set("physicsParams.timeStep", physicsParams.timeStep, 1);
	if (physicsParams.timeStep > inv60)
	{

		frame_count = time;
		physicsParams.timeStep;
		const float inv60_per_timeStep = inv60 / physicsParams.timeStep;

		physicsParams.timeStep /= physicsParams.calculate_iteration;

		for (int i = 0; i < physicsParams.calculate_iteration; i++) {

			// shapeのworld情報, physicsのtensorなどの更新
			update_world_trans(ALP_colliders[Sce]);

			// 外力の更新
			applyexternalforce(ALP_physicses[Sce], inv60_per_timeStep);

			pairs_new_num = 1 - pairs_new_num;

			// 大雑把な当たり判定
			Work_meter::start("Broad,Mid,Narrow", 1);

			Work_meter::start("Broadphase", 1);
			Work_meter::tag_start("Broadphase", 1);
			BroadMidphase(Sce,
				ALP_colliders[Sce], pairs[pairs_new_num],
				moved_collider_for_insertsort[Sce], added_collider_for_insertsort[Sce]
			);
			Work_meter::tag_stop(1);
			Work_meter::stop("Broadphase", 1);

			Work_meter::start("Midphase", 1);
			Work_meter::tag_start("Midphase", 1);
			Midphase(pairs[1 - pairs_new_num], pairs[pairs_new_num]);
			Work_meter::tag_stop(1);
			Work_meter::stop("Midphase", 1);

			// 衝突生成
			Work_meter::start("Narrowphase", 1);
			Work_meter::tag_start("Narrowphase", 1);
			generate_contact(pairs[pairs_new_num]);
			Work_meter::tag_stop(1);
			Work_meter::stop("Narrowphase", 1);

			Work_meter::stop("Broad,Mid,Narrow", 1);

			// 衝突解決
			Work_meter::start("Resolve", 1);
			Work_meter::tag_start("Resolve", 1);
			resolve_contact(ALP_colliders[Sce], pairs[pairs_new_num], ALP_joints);
			Work_meter::tag_stop(1);
			Work_meter::stop("Resolve", 1);

			{
				std::lock_guard <std::mutex> lock(mtx);
				// 計算中にgameobjectへtransformが適応されていた場合 ちゃんとtransformを更新してからintegrateを行う
				if (is_updated_mainthread) {
					is_updated_mainthread = false; // mainthreadがgameobject_transformを呼んだあと gameobjectのtransformをphysicsにコピーする

					//Colliderのframe毎に保存するdataをreset
					reset_data_per_frame(ALP_colliders[Sce], ALP_physicses[Sce]);
				}
				// 位置の更新
				integrate(ALP_physicses[Sce]);

				is_updated_physicsthread = true;
			}
		}

	}

	Work_meter::stop("Phyisics_manager", 1);

#ifdef Draw_JointContact
	static bool init = true;
	static const int size = 100;
	static Gameobject* debug_go[size];
	if (init) {
		for (int i = 0; i < size; i++) {
			debug_go[i] = Gameobject_manager::createSphere();
			debug_go[i]->transform->local_scale = Vector3(1) * 0.3f;
			if (i % 2 == 0)debug_go[i]->material->color = Vector4(1, 0.5f, 0.5f, 1);
			else debug_go[i]->material->color = Vector4(0.5f, 1, 0.5f, 1);

			debug_go[i]->is_hierarchy = false;
		}
		init = false;
	}

	for (int i = 0; i < size; i++) {
		debug_go[i]->transform->local_pos = Vector3(1000000, 1000000, 1000000);
	}

	int count = 0;
	for (const auto& p : ALP_joints) {

		count += 2;
		if (count > size)break;
		const auto coll0 = p->joint->get_colliderA();
		const auto coll1 = p->joint->get_colliderB();

		//debug_go[count - 1]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[0]), coll0.get_gameobject()->world_orientate()) + coll0.get_gameobject()->world_position();
		//debug_go[count - 2]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[1]), coll1.get_gameobject()->world_orientate()) + coll1.get_gameobject()->world_position();

		debug_go[count - 1]->transform->local_pos = coll0->gameobject->transform->position + vector3_quatrotate(p->limit_constraint_pos[0], coll0->gameobject->transform->orientation);
		debug_go[count - 2]->transform->local_pos = coll1->gameobject->transform->position + vector3_quatrotate(p->limit_constraint_pos[1], coll1->gameobject->transform->orientation);
		int adfsdg = 0;


	}

#endif // DEBUG

#ifdef Draw_Contact
	static bool init = true;
	static const int size = 100;
	static Gameobject* debug_go[size];
	if (init) {
		for (int i = 0; i < size; i++) {
			debug_go[i] = Gameobject_manager::createSphere();
			debug_go[i]->transform->local_scale = Vector3(1) * 0.3f;
			if (i % 2 == 0)debug_go[i]->material->color = Vector4(1, 0.5f, 0.5f, 1);
			else debug_go[i]->material->color = Vector4(0.5f, 1, 0.5f, 1);

			debug_go[i]->is_hierarchy = false;
		}
		init = false;
	}

	for (int i = 0; i < size; i++) {
		debug_go[i]->transform->local_pos = Vector3(1000000, 1000000, 1000000);
	}

	int count = 0;
	for (const auto& p : pairs[pairs_new_num]) {
		for (int i = 0; i < p->contacts.contact_num; i++) {
			count += 2;
			if (count > size)break;
			const auto coll0 = *p->body[0]->get_ALPcollider();
			const auto coll1 = *p->body[1]->get_ALPcollider();

			//debug_go[count - 1]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[0]), coll0.get_gameobject()->world_orientate()) + coll0.get_gameobject()->world_position();
			//debug_go[count - 2]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[1]), coll1.get_gameobject()->world_orientate()) + coll1.get_gameobject()->world_position();

			debug_go[count - 1]->transform->local_pos = p->body[0]->world_position() + vector3_quatrotate(p->contacts.contactpoints[i].point[0], p->body[0]->world_orientation());
			debug_go[count - 2]->transform->local_pos = p->body[1]->world_position() + vector3_quatrotate(p->contacts.contactpoints[i].point[1], p->body[1]->world_orientation());
			int adfsdg = 0;
			}
		}

#endif // DEBUG

	//is_updated_mainthread = false;
	//is_updated_physicsthread = true;
	return true;

	}


bool Physics_manager::update_Gui() {
	ImGuiWindowFlags flag = 0;
	//flag |= ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;

	if (ImGui::Begin("physics_global", 0, flag)) {

		//Colliderの表示
		ImGui::Checkbox("draw_collider", &is_draw_collider);
		ImGui::Checkbox("draw_DOP", &is_draw_dop);
		ImGui::Checkbox("draw_joint", &is_draw_joint);

		//重力の調整
		ImGui::InputFloat("gravity", &physicsParams.gravity, 0.1f, 1.0f, "%.2f");

		//正確さの調整
		ImGui::InputInt("solver_iteration", &physicsParams.solver_iteration, 1, 200);
		ImGui::InputInt("calculate_iteration", &physicsParams.calculate_iteration, 1, 200);

		//sleepの閾値
		ImGui::InputFloat("linear_sleep_threrhold", &physicsParams.linear_sleep_threrhold, 0.01f);
		ImGui::InputFloat("angular_sleep_threrhold", &physicsParams.angula_sleep_threrhold, 0.01f);

		//貫通時のばねの強さ
		ImGui::InputFloat("bias", &physicsParams.bias, 0.01f, 0.1f, "%.3f");
		//貫通許容誤差
		ImGui::InputFloat("slop", &physicsParams.slop, 0.0001f, 0.001f, "%.4f");

		//毎フレーム回すのはおもいので 何秒毎に回すのか
		ImGui::DragFloat("max_timeStep", &physicsParams.max_timeStep, 0.001f, 0.001f, 100000000);
		ImGui::Text("timeStep : %f", physicsParams.timeStep);

		//physics_defaultの表示
		if (ImGui::CollapsingHeader("physics_default")) {

			ImGui::DragFloat("mass", &physicsParams.inertial_mass, 0.1f);
			ImGui::DragFloat("linear_drag", &physicsParams.linear_drag, 0.01f);
			ImGui::DragFloat("anglar_drag", &physicsParams.anglar_drag, 0.01f);
			ImGui::DragFloat("dynamic_friction", &physicsParams.dynamic_friction, 0.01f);
			ImGui::DragFloat("static_friction", &physicsParams.static_friction, 0.01f);
			ImGui::DragFloat("restitution", &physicsParams.restitution, 0.01f);
			ImGui::Checkbox("is_fallable", &physicsParams.is_fallable);
			ImGui::Checkbox("is_kinmatic_anglar", &physicsParams.is_kinmatic_anglar);
			ImGui::Checkbox("is_kinmatic_linear", &physicsParams.is_kinmatic_linear);
			ImGui::Checkbox("is_moveable", &physicsParams.is_moveable);
			ImGui::Checkbox("is_hitable", &physicsParams.is_hitable);
		}

	}
	ImGui::End();

	return true;
}

bool Physics_manager::ray_cast(
	const Vector3& Ray_pos, const Vector3& Ray_dir,
	u_int tag,
	const float ray_min,
	float& tmin, float& tmax,
	Vector3& normal,
	Collider* ret_coll,
	Scenelist Sce) {

	std::lock_guard <std::mutex> lock(mtx);

	//tminからtmaxにかけてrayが交差している
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;
	float min = -FLT_MAX;
	float max = +FLT_MAX;
	Vector3 norm;

	bool ret = false;

	for (const auto coll : ALP_colliders[Sce]) {
		if (!(coll->tag & tag))continue;

		if (Physics_function::ray_cast(
			Ray_pos, Ray_dir,
			ray_min,
			coll,
			min, max, norm
		) == false) continue;

		ret = true;
		if (tmin > min) {
			tmin = min;
			normal = norm;
		}
		//tmin = ALmin(tmin, min);
		tmax = ALmax(tmax, max);
	}

	return ret;
}



void Physics_manager::thread_update() {
	while (is_stop_physics_thread == false)
	{
		update();
	}
}

void Physics_manager::adapt_added_data(Scenelist Sce, bool is_mutex_lock) {
	if (is_mutex_lock) mtx.lock(); //呼び出しのところでlockしている if文中なのでstd::lock_guard <std::mutex> lock(mtx)は使えない

	for (auto added_coll : added_buffer_ALP_colliders) {
		// 追加されたcolliderをbroadphase:sweep&prune用に
		for (auto& coll : added_coll.second) {
			coll->copy_transform_gameobject();
			added_collider_for_insertsort[added_coll.first].emplace_back(coll);
		}

		// 引っ越す
		int save_size = ALP_colliders[added_coll.first].size();
		ALP_colliders[added_coll.first].splice(ALP_colliders[added_coll.first].end(), std::move(added_coll.second));

		// listを引っ越したためitrが変化している 自身のitrを更新
		auto save_itr = ALP_colliders[added_coll.first].begin();
		for (save_itr; save_itr != ALP_colliders[added_coll.first].end(); ++save_itr) {
			save_size--;
			if (save_size >= 0)continue;
			(*save_itr)->set_this_itr(save_itr);

			// メイン配列に引っ越したためis_addedをtrueに
			(*save_itr)->is_added = true;
		}
	}
	for (auto& added_phys : added_buffer_ALP_physicses) {

		int save_size = ALP_physicses[added_phys.first].size();
		ALP_physicses[added_phys.first].splice(ALP_physicses[added_phys.first].end(), std::move(added_phys.second));

		// listを引っ越したためitrが変化している 自身のitrを更新
		auto save_itr = ALP_physicses[added_phys.first].begin();
		for (save_itr; save_itr != ALP_physicses[added_phys.first].end(); ++save_itr) {
			save_size--;
			if (save_size >= 0)continue;
			(*save_itr)->set_this_itr(save_itr);

			// メイン配列に引っ越したためis_addedをtrueに
			(*save_itr)->is_added = true;
		}
	}

	added_buffer_ALP_colliders.clear();
	added_buffer_ALP_physicses.clear();

	for (auto added_coll : ALP_colliders[Sce]) {
		// 各colliderのshapeのadded_dataを処理
		added_coll->adapt_added_data();
	}

	{
		// 引っ越す
		ALP_joints.splice(ALP_joints.end(), std::move(added_buffer_ALP_joints));

		// listを引っ越したためitrが変化している 自身のitrを更新
		auto save_itr = ALP_joints.begin();
		for (save_itr; save_itr != ALP_joints.end(); ++save_itr) {
			(*save_itr)->set_this_itr(save_itr);

			// メイン配列に引っ越したためis_addedをtrueに
			(*save_itr)->is_added = true;
		}
	}

	if (is_mutex_lock) mtx.unlock();
}

void Physics_manager::dadapt_delete_data(bool is_mutex_lock) {
	if (is_mutex_lock) mtx.lock(); //呼び出しのところでlockしている if文中なのでstd::lock_guard <std::mutex> lock(mtx)は使えない

	for (auto deleted_coll : dalated_buffer_ALP_colliders) {
		// 削除されたcollider
		for (auto& coll : deleted_coll.second) {
			coll->destroy();
			delete coll;
		}
	}


	for (auto& deleted_phys : dalated_buffer_ALP_physicses) {
		// 削除されたphysics
		for (auto& phys : deleted_phys.second) {
			phys->destroy();
			delete phys;
		}
	}

	dalated_buffer_ALP_colliders.clear();
	dalated_buffer_ALP_physicses.clear();

	for (auto& deleted_joint : dalated_buffer_ALP_joints) {
		deleted_joint->destroy(nullptr, true);
		delete deleted_joint;
	}
	dalated_buffer_ALP_joints.clear();

	if (is_mutex_lock) mtx.unlock();
}

bool Physics_manager::adapt_transform_to_gameobject(Scenelist Sce) {
	std::lock_guard <std::mutex> lock(mtx);

	count_mainthread += 1;

	if (is_updated_physicsthread == false) return false;

	for (auto coll : ALP_colliders[Sce]) {
		coll->adapt_to_gameobject_transform();
	}

	is_updated_physicsthread = false;

	return true;
}

void Physics_manager::copy_gameobject_transform(Scenelist Sce) {
	std::lock_guard <std::mutex> lock(mtx);

	for (auto& coll : ALP_colliders[Sce]) {
		coll->copy_transform_gameobject();
	}

	Physics_manager::is_updated_mainthread = true;
}





// 別threadでupdateを回す
void Physics_manager::thread_start() {
	is_stop_physics_thread = false;
	is_updated_mainthread = false;
	is_updated_physicsthread = false;
	physics_thread = std::thread(thread_update);
}

// 別threadでのupdateを止めて、joinを行う
void Physics_manager::thread_stop_and_join() {
	is_stop_physics_thread = true;
	is_updated_mainthread = true;
	if (physics_thread.joinable())physics_thread.join();
}



#include "../Main/systems.h"
#include "../Renderer/material_for_collider.h"

bool Physics_manager::render_collider(Scenelist Sce) {
	render_dop(Sce);
	render_joint(Sce);

	if (is_draw_collider == false)return false;

	for (auto& itr : ALP_colliders[Sce]) {
		Collider_renderer::render_collider(itr);
	}

	return true;
}

bool Physics_manager::render_dop(Scenelist Sce) {
	if (is_draw_dop == false)return false;

	for (auto& itr : ALP_colliders[Sce]) {
		Collider_renderer::render_AABB(itr);
	}

	return true;

}

bool Physics_manager::render_joint(Scenelist Sce) {

	if (is_draw_joint == false)return false;

	for (auto& itr : ALP_joints) {
		Collider_renderer::render_joint(itr);
	}

	return true;
}

void Physics_manager::destroy(Scenelist Sce) {

	dadapt_delete_data();

	for (auto& joint : ALP_joints) {
		auto& base = joint->userjoint;
		base->destroy();
		delete base;
	}


	//for (auto& joint : added_ALP_joints) {
	//	auto& base = joint->userjoint;
	//	base->destroy();
	//	delete base;
	//}
	//for (auto& coll_pair : added_ALP_colliders) {
	//	for (auto& coll : coll_pair.second) {
	//		coll->destroy();
	//		delete coll;
	//	}
	//}
	//for (auto& phys_pair : added_ALP_physicses) {
	//	for (auto& phys : phys_pair.second) {
	//		phys->destroy();
	//		delete phys;
	//	}
	//}



	for (auto& p : pairs[0]) {
		delete p;
	}
	for (auto& p : pairs[1]) {
		delete p;
	}
	pairs[0].clear();
	pairs[1].clear();
}

