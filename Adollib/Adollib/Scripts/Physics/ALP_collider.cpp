#include "ALP_Collider.h"

#include "collider__base.h"
#include "../Object/gameobject.h"

#include "ALP__physics_manager.h"

#include "../Object/gameobject.h"

#include "collider_shape.h"

#include "ALP_physics.h"

using namespace Adollib;
using namespace Physics_function;


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region Collider
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
const bool ALP_Collider::concoll_enter(const unsigned int tag_name) {
	oncoll_check_bits |= tag_name;
	return (oncoll_bits & tag_name);
}
void ALP_Collider::solv_resolve() {
	////offset_CollGO_quat = world_orientation() * gameobject->get_world_orientate().inverse() * local_orientation.inverse();
	////offset_CollGO_quat = world_orientation() * (local_orientation * gameobject->get_world_orientate()).inverse();
	//offset_CollGO_quat = (local_orientation * gameobject->get_world_orientate()).inverse() * world_orientation();
	////offset_CollGO_quat = local_orientation.inverse() * gameobject->get_world_orientate().inverse() * world_orientation();
	//offset_CollGO_pos = world_position() - vector3_quatrotate(local_position * gameobject->get_world_scale(), world_orientation()) - gameobject->get_world_position();

	buffer_quat_chang = quaternion_identity();
	buffer_pos_chang = Vector3(0);

	for (const auto& shape : shapes) {
		buffer_quat_chang *= (shape->local_orientation * gameobject->world_orientate()).inverse() * shape->world_orientation();
		buffer_pos_chang += shape->world_position() - vector3_quatrotate(shape->local_position * gameobject->world_scale(), shape->world_orientation()) - gameobject->world_position();
	}
}

void ALP_Collider::resolve_gameobject() {
	gameobject->transform->local_orient = gameobject->transform->local_orient * buffer_quat_chang;
	gameobject->transform->local_pos += buffer_pos_chang;
}

void ALP_Collider::update_world_trans() {
	bool is_changed_Size = false;

	bool is_changPos = false;
	bool is_changRot = false;
	bool is_changSiz = false;

	for (auto& shape : shapes) {

		//ユーザーに入力されたcolliderデータ(center,sizeなど)を計算用のデータに治す
		shape->adapt_Colliderdata();

		//world情報の更新
		shape->update_world_trans(gameobject->world_position(), gameobject->world_orientate(), gameobject->world_scale(),
			is_changPos, is_changRot, is_changSiz
		);

		is_changed_Size |= is_changSiz;

		//DOPの更新
		shape->update_dop14();
	}

	//ユーザーに入力されたphysicsデータ(massなど)を計算用のデータに治す
	ALPphysics->update_physics_data();

	//慣性モーメントの更新
	ALPphysics->update_tensor(shapes);




}

#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void ALP_Collider::update_dop14() {
	for (auto& shape : shapes) {
		shape->update_dop14();
	}
}

void ALP_Collider::integrate(float duration, Vector3 linear_velocity, Vector3 anglar_velocity) {
	if (linear_velocity.norm() == 0 && anglar_velocity.norm() == 0)return;

	//親のorienattionの逆をとる
	Quaternion pearent_orientate_inv = Quaternion(1, 0, 0, 0);
	if (gameobject->pearent() != nullptr) {
		pearent_orientate_inv = gameobject->pearent()->world_orientate();
		pearent_orientate_inv = pearent_orientate_inv.inverse();
	}

	//位置の更新
	gameobject->transform->local_orient = gameobject->transform->local_orient * buffer_quat_chang;
	gameobject->transform->local_pos += buffer_pos_chang;

	//アタッチされているGOの親子関係に対応 親が回転していても落下は"下"方向に
	Vector3 local_linear_velocity = vector3_quatrotate(linear_velocity, pearent_orientate_inv);
	Vector3 local_anglar_velocity = vector3_quatrotate(anglar_velocity, pearent_orientate_inv);

	gameobject->transform->local_pos += local_linear_velocity * duration;

	gameobject->transform->local_orient *= quaternion_radian_axis(local_anglar_velocity.norm_sqr() * duration * 0.5f, local_anglar_velocity.unit_vect());
	gameobject->transform->local_orient = gameobject->transform->local_orient.unit_vect();
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

void ALP_Collider::set_inertial_mass(const Matrix& m) { ALPphysics->inertial_tensor = m; };