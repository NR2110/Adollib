#include "ALP_Collider.h"

#include "../Object/gameobject.h"
#include "ALP__physics_manager.h"

#include "collider_shape.h"
#include "ALP_physics.h"
#include "ALP__meshcoll_data.h"


using namespace Adollib;
using namespace Physics_function;


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Collider
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
const bool ALP_Collider::concoll_enter(const unsigned int tag_name) {
	oncoll_check_bits |= tag_name;
	return (oncoll_bits & tag_name);
}

void ALP_Collider::update_world_trans() {
	bool is_changed_Size = false;

	bool is_changPos = false;
	bool is_changRot = false;
	bool is_changSiz = false;

	for (auto& shape : shapes) {

		// ユーザーに入力されたcolliderデータ(center,sizeなど)を計算用のデータに治す
		shape->update_Colliderdata();

		// world情報の更新
		shape->update_world_trans(gameobject->world_position(), gameobject->world_orientate(), gameobject->world_scale(),
			is_changPos, is_changRot, is_changSiz
		);

		is_changed_Size |= is_changSiz;

		// DOPの更新
		shape->update_dop14();

	}


	// ユーザーに入力されたphysicsデータ(massなど)を計算用のデータに治す
	ALPphysics->update_physics_data();

	// 慣性モーメントの更新
	ALPphysics->update_tensor_and_mass(shapes);




}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void ALP_Collider::integrate(float duration, Vector3 linear_velocity, Vector3 anglar_velocity) {
	if (linear_velocity.norm() == 0 && anglar_velocity.norm() == 0)return;

	//親のorienattionの逆をとる
	Quaternion pearent_orientate_inv = Quaternion(1, 0, 0, 0);
	if (gameobject->pearent() != nullptr) {
		pearent_orientate_inv = gameobject->pearent()->world_orientate();
		pearent_orientate_inv = pearent_orientate_inv.inverse();
	}

	//アタッチされているGOの親子関係に対応 親が回転していても落下は"下"方向に
	Vector3 local_linear_velocity = vector3_quatrotate(linear_velocity, pearent_orientate_inv);
	Vector3 local_anglar_velocity = vector3_quatrotate(anglar_velocity, pearent_orientate_inv);

	gameobject->transform->local_pos += local_linear_velocity * duration;

	Quaternion debug_0, debug_1;

	//どっちも計算結果は一緒 上のほうが直感的
	gameobject->transform->local_orient *= quaternion_radian_axis(local_anglar_velocity.norm_sqr() * duration , local_anglar_velocity.unit_vect());
	gameobject->transform->local_orient = gameobject->transform->local_orient.unit_vect();

	//Quaternion dAng = gameobject->transform->local_orient.unit_vect() * Quaternion(0, anglar_velocity.x, anglar_velocity.y, anglar_velocity.z) * 0.5f;
	//gameobject->transform->local_orient =( gameobject->transform->local_orient + dAng * duration).unit_vect();

}

void ALP_Collider::Update_hierarchy()
{
	for (auto shape : shapes) {
		shape->Update_hierarchy();
	}
};

Meshcoll_part* ALP_Collider::add_mesh_shape(const char* filepass, Physics_function::Meshcollider_data* mesh_data) {
	Meshcoll_part* shape = newD Meshcoll_part(this, filepass, mesh_data);

	shapes.emplace_back(shape);
	return shape;
};

void ALP_Collider::destroy()
{
	//shapeの解放
	for (auto shape : shapes) {
		delete shape;
	}

	Phyisics_manager::remove_ALPcollider(scene, this_itr);
};

const Collider_tagbit ALP_Collider::get_tag()const { return (*coll_itr)->tag; }; //自身のtag(bit)
const Collider_tagbit ALP_Collider::get_ignore_tags() const { return (*coll_itr)->ignore_tags; }; //衝突しないtags

//void ALP_Collider::update_AABB() {
//
//	DOP::AABB aabb;
//	aabb.max = Vector3(-FLT_MAX);
//	aabb.min = Vector3(+FLT_MAX);
//
//	// shapeがアタッチされていなければ0を入れて返す
//	if (shapes.size() == 0) {
//		aabb.max = Vector3(0);
//		aabb.min = Vector3(0);
//		aabb.pos = Vector3(0);
//		AABB = aabb;
//		return;
//	}
//
//	// shapesの最大,最小の点を求める
//	for (const auto& shape : shapes) {
//		const DOP::DOP_14& dop = shape->get_DOP();
//
//		for (int i = 0; i < 3; i++) {
//			if (aabb.max[i] < dop.max[i] + dop.pos[i])aabb.max[i] = dop.max[i] + dop.pos[i];
//			if (aabb.min[i] > dop.min[i] + dop.pos[i])aabb.min[i] = dop.min[i] + dop.pos[i];
//		}
//
//	}
//	// 中心座標の調整 いずれ質量の中心をposに持ってくるかも????
//	aabb.pos = Vector3(
//		(aabb.max[0] + aabb.min[0]) * 0.5f,
//		(aabb.max[1] + aabb.min[1]) * 0.5f,
//		(aabb.max[2] + aabb.min[2]) * 0.5f
//	);
//
//	// min,maxを調整
//	for (int i = 0; i < 3; i++) {
//		const float len = fabsf(aabb.max[i] - aabb.pos[i]);
//
//		aabb.max[i] = +len;
//		aabb.min[i] = -len;
//	}
//
//	AABB = aabb;
//}