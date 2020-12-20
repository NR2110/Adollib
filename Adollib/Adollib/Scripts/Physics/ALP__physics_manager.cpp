#include "ALP__physics_function.h"
#include "ALP__physics_manager.h"
#include "../Scene/scene.h"
#include "../Main/Adollib.h"

#include "../Object/gameobject_manager.h"
#include "../Imgui/imgui_all.h"
#include "../Imgui/work_meter.h"

#include "collider.h"

#include "ALP_raycast.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;

#define Draw_Contact

//::: staticメンバの初期化 :::::
namespace Adollib
{
	float Phyisics_manager::gravity = Physics_manager_default::gravity; //重力

	Physics_function::ALP_Physics Phyisics_manager::default_physics = ALP_Physics(
		Physics_manager_default::inertial_mass, //質量
		Physics_manager_default::drag, //空気抵抗
		Physics_manager_default::anglar_drag, //空気抵抗
		Physics_manager_default::dynamic_friction,//動摩擦
		Physics_manager_default::static_friction, //静摩擦
		Physics_manager_default::restitution,	 //反発係数

		Physics_manager_default::is_fallable, //落ちない
		Physics_manager_default::is_kinematic, //影響うけない(fallはする)
		Physics_manager_default::is_kinmatic_anglar, //ほかの物体からの影響で回転速度が変化しない
		Physics_manager_default::is_kinmatic_linear, //ほかの物体からの影響で並進速度が変化しない
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

	float Phyisics_manager::timeStep = 0.016f;

	bool Phyisics_manager::is_draw_collider = false;
	bool Phyisics_manager::is_draw_dop = false;
}

namespace Adollib
{
	int Phyisics_manager::frame_count;

	//collider_componentのポインタ配列
	std::unordered_map<Scenelist, std::list<Collider*>> Phyisics_manager::colliders;

	//各dataの実態配列
	std::unordered_map<Scenelist, std::list<Physics_function::ALP_Collider>> Phyisics_manager::ALP_colliders;
	std::unordered_map<Scenelist, std::list<Physics_function::ALP_Physics>> Phyisics_manager::ALP_physicses;

	std::vector<Physics_function::Contacts::Contact_pair> Phyisics_manager::pairs;
	std::vector<Physics_function::Contacts::Collider_2> Phyisics_manager::broad_mid_pair;

	bool Phyisics_manager::is_changed_coll = false; //新たにColliderが追加された、削除された場合true
}



bool Phyisics_manager::update(Scenelist Sce)
{
	update_Gui();

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
	Broadphase(ALP_colliders[Sce], broad_mid_pair);
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
		}
		init = false;
	}

	for (int i = 0; i < size; i++) {
		debug_go[i]->transform->local_pos = Vector3(1000000, 1000000, 1000000);
	}

	int count = 0;
	for (const auto& p : pairs) {
		for (int i = 0; i < p.contacts.contact_num; i++) {
			count += 2;
			if (count > size)break;
			const auto* coll0 = (*p.body[0]->ALPcollider->coll_itr)->transform;
			const auto* coll1 = (*p.body[1]->ALPcollider->coll_itr)->transform;

			debug_go[count - 1]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[0]/* * coll0->scale*/), coll0->orientation) + coll0->position;
			debug_go[count - 2]->transform->local_pos = vector3_quatrotate((p.contacts.contactpoints[i].point[1]/* * coll1->scale*/), coll1->orientation) + coll1->position;
			int adfsdg = 0;
		}
	}

#endif // DEBUG
	return true;

}


bool Phyisics_manager::update_Gui() {
	ImGuiWindowFlags flag = 0;
	//flag |= ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;

	if (ImGui::Begin("physics_global", 0, flag)) {

		//Colliderの表示
		ImGui::Checkbox("draw_collider", &is_draw_collider);
		//Colliderの表示
		ImGui::Checkbox("draw_DOP", &is_draw_dop);

		//重力の調整
		ImGui::InputFloat("gravity", &gravity, 0.1f, 1.0f, "%.2f");

		//正確さの調整
		ImGui::InputInt("accuracy", &solver_iterations, 1, 200);

		//貫通時のばねの強さ
		ImGui::InputFloat("bias", &bias, 0.01f, 0.1f, "%.3f");
		//貫通許容誤差
		ImGui::InputFloat("slop", &slop, 0.0001f, 0.001f, "%.4f");

		//physics_defaultの表示
		if (ImGui::CollapsingHeader("physics_default")) {

			ImGui::Columns(2);
			ImGui::Separator();
			ImGui::Separator();
			ImGui::Text("mass"); ImGui::NextColumn();  ImGui::Text("%f", Physics_manager_default::inertial_mass); ImGui::NextColumn();
			ImGui::Text("drag"); ImGui::NextColumn();  ImGui::Text("%f", Physics_manager_default::drag); ImGui::NextColumn();
			ImGui::Text("anglar_drag"); ImGui::NextColumn();  ImGui::Text("%f", Physics_manager_default::anglar_drag); ImGui::NextColumn();
			ImGui::Text("dynamic_friction"); ImGui::NextColumn();  ImGui::Text("%f", Physics_manager_default::dynamic_friction); ImGui::NextColumn();
			ImGui::Text("static_friction"); ImGui::NextColumn();  ImGui::Text("%f", Physics_manager_default::static_friction); ImGui::NextColumn();
			ImGui::Text("restitution"); ImGui::NextColumn();  ImGui::Text("%f", Physics_manager_default::restitution); ImGui::NextColumn();
			ImGui::Text("is_fallable"); ImGui::NextColumn();  if (Physics_manager_default::is_fallable)ImGui::Text("true"); else ImGui::Text("false"); ImGui::NextColumn();
			ImGui::Text("is_kinematic"); ImGui::NextColumn(); if (Physics_manager_default::is_kinematic)ImGui::Text("true"); else ImGui::Text("false"); ImGui::NextColumn();
			ImGui::Text("is_moveable"); ImGui::NextColumn();  if (Physics_manager_default::is_moveable)ImGui::Text("true"); else ImGui::Text("false"); ImGui::NextColumn();
			ImGui::Text("is_hitable"); ImGui::NextColumn();   if (Physics_manager_default::is_hitable)ImGui::Text("true"); else ImGui::Text("false"); ImGui::NextColumn();
			ImGui::Separator();

			ImGui::Columns(1);
		}

	}
	ImGui::End();

	return true;
}

bool Phyisics_manager::ray_cast(
	const Vector3& Ray_pos, const Vector3& Ray_dir,
	u_int tag,
	const float ray_min,
	float& tmin, float& tmax,
	Vector3& normal,
	Collider* ret_coll,
	Scenelist Sce) {

	//tminからtmaxにかけてrayが交差している
	tmin = +FLT_MAX;
	tmax = -FLT_MAX;
	float min = -FLT_MAX;
	float max = +FLT_MAX;
	Vector3 norm;

	bool ret = false;

	for (const auto& coll : ALP_colliders[Sce]) {
		if (!(coll.tag | tag))continue;

		if (Physics_function::ray_cast(
			Ray_pos, Ray_dir,
			coll,
			ray_min,
			min, max, norm
		) == false) continue;

		ret = true;
		if(tmin > min){
			tmin  = min;
			normal = norm;
		}
		//tmin = ALmin(tmin, min);
		tmax = ALmax(tmax, max);
	}

	return ret;
}

#include "../Main/systems.h"
#include "../Mesh/material_for_collider.h"

bool Phyisics_manager::render_collider(Scenelist Sce) {
	render_dop(Sce);

	if (is_draw_collider == false)return false;

	for (auto& itr : ALP_colliders[Sce]) {
		Collider_renderer::render_collider(itr);
	}

	return true;
}

bool Phyisics_manager::render_dop(Scenelist Sce) {
	if (is_draw_dop == false)return false;

	for (auto& itr : ALP_colliders[Sce]) {
		Collider_renderer::render_AABB(itr);
	}

	return true;

}

void Phyisics_manager::destroy(Scenelist Sce) {

	colliders[Sce].clear();
}

