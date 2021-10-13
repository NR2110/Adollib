
#include "ALP_collider.h"

#include "../Object/gameobject.h"
#include "ALP__physics_manager.h"

#include "collider_shape.h"
#include "ALP_physics.h"
#include "ALP_joint.h"
#include "ALP__meshcoll_data.h"

using namespace Adollib;
using namespace Physics_function;


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Collider
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
const bool ALP_Collider::concoll_enter(const unsigned int tag_name) {
	//std::lock_guard <std::mutex> lock(mtx); //oncoll_check_bitsはphysicsで変更されないため

	oncoll_check_bits |= tag_name;
	return (oncoll_bits & tag_name);
}

void ALP_Collider::update_world_trans() {
	std::lock_guard <std::mutex> lock(mtx); //userの任意のタイミングで呼ぶことが可能なためshapesのaddedの処理が重なる可能性がある

	bool is_changed_Size = false;

	bool is_changPos = false;
	bool is_changRot = false;
	bool is_changSiz = false;

	for (const auto& shape : shapes) {

		// ユーザーに入力されたcolliderデータ(center,sizeなど)を計算用のデータに直す
		shape->update_Colliderdata();

		// world情報の更新
		shape->update_world_trans(transform.position, transform.orientation, transform.scale,
			is_changPos, is_changRot, is_changSiz
		);

		is_changed_Size |= is_changSiz;

		// DOPの更新
		shape->update_dop14();
	}

	// 慣性モーメントの更新
	ALPphysics->update_tensor_and_barycenter(shapes, joints);
}

void ALP_Collider::update_world_trans_contain_added() {
	std::lock_guard <std::mutex> lock(mtx); //shapes,addedの処理が重なる可能性がある

	bool is_changPos = false;
	bool is_changRot = false;
	bool is_changSiz = false;


	for (const auto& shape : shapes) {

		// ユーザーに入力されたcolliderデータ(center,sizeなど)を計算用のデータに治す
		shape->update_Colliderdata();

		// world情報の更新
		shape->update_world_trans(transform.position, transform.orientation, transform.scale,
			is_changPos, is_changRot, is_changSiz
		);
	}
	for (const auto& shape : added_shapes) {

		// ユーザーに入力されたcolliderデータ(center,sizeなど)を計算用のデータに治す
		shape->update_Colliderdata();

		// world情報の更新
		shape->update_world_trans(transform.position, transform.orientation, transform.scale,
			is_changPos, is_changRot, is_changSiz
		);
	}
}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void ALP_Collider::integrate(const float duration, const Vector3& linear_velocity, const Vector3& angula_velocity, const Vector3& old_linear_velocity, const Vector3& old_angula_velocity) {
	if (linear_velocity.norm() == 0 && angula_velocity.norm() == 0)return;

#if 0
	//std::lock_guard <std::mutex> lock(mtx); //

	if (linear_velocity.norm() == 0 && angula_velocity.norm() == 0)return;

	//親のorientationの逆をとる
	Quaternion pearent_orientate_inv = Quaternion(1, 0, 0, 0);
	if (gameobject->pearent() != nullptr) {
		pearent_orientate_inv = gameobject->pearent()->world_orientate();
		pearent_orientate_inv = pearent_orientate_inv.inverse();
	}

	//現在の速度が 前の速度から加速度一定で変化したとして移動距離を求める
	const Vector3 linear_move = old_linear_velocity * duration + 0.5f * (linear_velocity - old_linear_velocity) * duration;
	const Vector3 angula_move = old_angula_velocity * duration + 0.5f * (angula_velocity - old_angula_velocity) * duration;

	//アタッチされているGOの親子関係に対応 親が回転していても落下は"下"方向に
	const Vector3 local_linear_move = vector3_quatrotate(linear_move, pearent_orientate_inv);
	const Vector3 local_anglar_move = vector3_quatrotate(angula_move, pearent_orientate_inv);

	// transformに適応する
	Vector3 local_linear_velocity = vector3_quatrotate(linear_velocity, pearent_orientate_inv);
	gameobject->transform->local_pos += local_linear_velocity * duration;
	//gameobject->transform->local_pos += local_linear_move;

	Vector3 local_anglar_velocity = vector3_quatrotate(angula_velocity, pearent_orientate_inv);
	gameobject->transform->local_orient *= quaternion_axis_radian(local_anglar_velocity.unit_vect(), local_anglar_velocity.norm_sqr() * duration);
	//gameobject->transform->local_orient *= quaternion_axis_radian(local_angula_move.unit_vec(), local_angula_move.norm_sqr());
	gameobject->transform->local_orient = gameobject->transform->local_orient.unit_vect();
#else
	//親のorientationの逆をとる
	Quaternion parent_orientate_inv = Quaternion(1, 0, 0, 0);
	//if (gameobject->parent() != nullptr) {
	//	parent_orientate_inv = gameobject->parent()->world_orientate();
	//	parent_orientate_inv = parent_orientate_inv.inverse();
	//}

	//現在の速度が 前の速度から加速度一定で変化したとして移動距離を求める
	const Vector3 linear_move = old_linear_velocity * duration + 0.5f * (linear_velocity - old_linear_velocity) * duration;
	const Vector3 angula_move = old_angula_velocity * duration + 0.5f * (angula_velocity - old_angula_velocity) * duration;

	//アタッチされているGOの親子関係に対応 親が回転していても落下は"下"方向に
	const Vector3 local_linear_move = vector3_quatrotate(linear_move, parent_orientate_inv);
	const Vector3 local_angula_move = vector3_quatrotate(angula_move, parent_orientate_inv);

	// transformに適応する
	const Vector3 local_linear_velocity = vector3_quatrotate(linear_velocity, parent_orientate_inv);
	transform.position += local_linear_velocity * duration;

	const Vector3 local_angula_velocity = vector3_quatrotate(angula_velocity, parent_orientate_inv);
	transform.orientation *= quaternion_axis_radian(local_angula_velocity.unit_vect(), local_angula_velocity.norm_sqr() * duration);
	transform.orientation = transform.orientation.unit_vect();

#endif
}

void ALP_Collider::reset_data_per_frame() {
	//std::lock_guard <std::mutex> lock(mtx);
	//
	if (is_deleted)return;  //goがすでにdeleteされていればreturn

	oncoll_bits = 0;

	start_transform.position = gameobject->transform->position;
	start_transform.orientation = gameobject->transform->orientation;
	start_transform.scale = gameobject->transform->scale;

	transform.position = start_transform.position;
	transform.orientation = start_transform.orientation;
	transform.scale = start_transform.scale;
};

void ALP_Collider::adapt_collider_component_data() {
	tag = coll_ptr->tag;
	ignore_tags = coll_ptr->ignore_tags;
	coll_ptr->contacted_colliders.clear();
}

void ALP_Collider::Update_hierarchy(){
	std::lock_guard <std::mutex> lock(mtx); //shapesに範囲forでアクセスするため

	for (const auto& shape : shapes) {
		shape->Update_hierarchy();
	}
};

void ALP_Collider::adapt_to_gameobject_transform() const
{
	// mainthreaddから呼ばれるためここでis_deletedチェックを行えばよい
	if (is_deleted) return; // gameobjectが削除されていたらreturn

	//親のorientationの逆をとる
	Quaternion parent_orientate_inv = Quaternion(1, 0, 0, 0);
	if (gameobject->parent() != nullptr) {
		parent_orientate_inv = gameobject->parent()->world_orientate();
		parent_orientate_inv = parent_orientate_inv.inverse();
	}

	gameobject->transform->local_pos += vector3_quatrotate(transform.position - start_transform.position, parent_orientate_inv);
	const Quaternion buffer = start_transform.orientation.inverse() * transform.orientation;
	gameobject->transform->local_orient *= quaternion_axis_radian(vector3_quatrotate(buffer.axis(), parent_orientate_inv), buffer.radian());
	//gameobject->transform->local_scale *= transform.scale / start_transform.scale;
}

//:::::

//このcolliderが属するjointを追加 (destroyのため)
void ALP_Collider::add_joint(ALP_Joint* joint) {
	//std::lock_guard <std::mutex> lock(mtx);

#if _DEBUG
	//エラー表示 同じjointをアタッチしてはいけない
	for (auto& j : joints) {
		if (j == joint) assert(0 && " the joint is already been added");
	}
#endif
	joints.emplace_back(joint);
}

//jointから外された
void ALP_Collider::remove_joint(ALP_Joint* joint) {
	//std::lock_guard <std::mutex> lock(mtx);

	joints.remove(joint);
}

void ALP_Collider::add_contacted_collider(const Contacts::Contact_pair* pair, const u_int num) const
{
	//TODO : contacted_collidersをALPcolliderが持つように変更する
	return;

	if (is_deleted)return; //GOが削除されていたらcollider::componentも存在しないためreturn
	if (coll_ptr->is_save_contacted_colls == false) return; //保存するflagが立っていなければ保存しない

	Contacted_data data;
	data.coll = pair->body[1 - num]->get_ALPcollider()->get_collptr();
	//すべてのpairから衝突点、貫通量、衝突法線を保存
	for (int contact_num = 0; contact_num < pair->contacts.contact_num; ++contact_num) {
		const auto& contact_point = pair->contacts.contactpoints[contact_num];

		//shape座標系からGO座標系に直して保存(縦を合わせたいから"0+"を入れている)
		data.contacted_pointA = vector3_quatrotate(contact_point.point[0 + num], pair->body[0 + num]->local_orientation) + pair->body[0 + num]->local_position;
		data.contacted_pointB = vector3_quatrotate(contact_point.point[1 - num], pair->body[1 - num]->local_orientation) + pair->body[1 - num]->local_position;
		data.normal = contact_point.normal; //ワールド情報
		data.penetrate = contact_point.distance;

		coll_ptr->contacted_colliders.push_back(data);
	}

}

Meshcoll_part* ALP_Collider::add_mesh_shape(const char* filepath, Physics_function::Meshcollider_data* mesh_data) {
	std::lock_guard <std::mutex> lock(mtx);
	auto shape = newD Meshcoll_part(this, filepath, mesh_data);

	added_shapes.emplace_back(shape);
	return shape;
};

void ALP_Collider::destroy(){
	//std::lock_guard <std::mutex> lock(mtx);

	//shapeの解放
	for (const auto& shape : shapes) {
		delete shape;
	}

	//jointの削除
	for (const auto& j : joints) {
		j->destroy(this, true);
		delete j;
	}

	//physics_managerの配列から削除
	Physics_manager::remove_ALPcollider(scene, this_itr);

};

Collider_tagbit ALP_Collider::get_tag()const { return coll_ptr->tag; }; //自身のtag(bit)
Collider_tagbit ALP_Collider::get_ignore_tags() const { return coll_ptr->ignore_tags; }; //衝突しないtags