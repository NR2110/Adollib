#include "stage_base.h"

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Physics/joint.h"

#include "kinematic_block.h"
#include "area_respown_set.h"
#include "area_goal.h"

using namespace Adollib;
using namespace Stage_parts;


Collider* Stage_base::set_sphere(const Vector3& pos, const float& r, const Vector3& color, Gameobject* pearent, bool is_static ) {

	Gameobject* object = nullptr;
	object = Gameobject_manager::createSphere("sphere");

	object->renderer->color = Vector4(color.x, color.y, color.z , 1);
	object->transform->local_pos = pos;
	object->transform->local_scale = Vector3(r, r, r);

	Collider* coll = object->addComponent<Collider>();
	coll->add_shape<Sphere>();

	if (is_static) {
		coll->tag = Collider_tags::Sphere | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Static_Stage ;
		coll->physics_data.is_moveable = false;
	}
	else {
		coll->tag = Collider_tags::Sphere | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Kinematic_Stage ;
		// 動くのであれば所定のcomponentをアタッチする
		auto comp = object->addComponent<Kinematic_block>();
		comp->this_stage = this;
		comp->this_coll = coll;
		comp->respown_pos = pos;
		comp->respown_rotate = quaternion_identity();

	}

	//stage_parts.emplace_back(object);
	if (pearent != nullptr)pearent->add_child(object);
	return coll;
}

Collider* Stage_base::set_box(const Vector3& pos, const Vector3& size, const Vector3& rotate, const Vector3& color, Gameobject* pearent, bool is_static ) {
	Gameobject* object = nullptr;
	object = Gameobject_manager::createCube("Cube");
	object->renderer->color = Vector4(color.x, color.y, color.z, 1);

	object->transform->local_orient = quaternion_from_euler(rotate);
	object->transform->local_pos = pos;
	object->transform->local_scale = size;

	Collider* coll = object->addComponent<Collider>();
	coll->add_shape<Box>();

	if (is_static) {
		coll->tag = Collider_tags::Box | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Static_Stage ;
		coll->physics_data.is_moveable = false;
	}
	else {
		coll->tag = Collider_tags::Box | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Kinematic_Stage ;
		// 動くのであれば所定のcomponentをアタッチする
		auto comp = object->addComponent<Kinematic_block>();
		comp->this_stage = this;
		comp->this_coll = coll;
		comp->respown_pos = pos;
		comp->respown_rotate = object->transform->local_orient;
		coll->physics_data.inertial_mass = size.x * size.y * size.z;

	}

	//stage_parts.emplace_back(object);
	if (pearent != nullptr)pearent->add_child(object);
	return coll;
}

Collider* Stage_base::set_capsule(const Vector3& pos, const float& r, const float& length, const Vector3& rotate, const Vector3& color, Gameobject* pearent, bool is_static ) {
	Gameobject* object = nullptr;
	object = Gameobject_manager::createCube("capsule");
	object->renderer->color = Vector4(color.x, color.y, color.z, 1);

	object->transform->local_orient = quaternion_from_euler(rotate);
	object->transform->local_pos = pos;
	object->transform->local_scale = Vector3(r, r + length, r);

	Collider* coll = object->addComponent<Collider>();
	Capsule* capsule = coll->add_shape<Capsule>();
	capsule->length = length * 0.5f;

	if (is_static) {
		coll->tag = Collider_tags::Capsule | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Static_Stage ;
		coll->physics_data.is_moveable = false;
	}
	else {
		coll->tag = Collider_tags::Capsule | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Kinematic_Stage ;
		// 動くのであれば所定のcomponentをアタッチする
		auto comp = object->addComponent<Kinematic_block>();
		comp->this_stage = this;
		comp->this_coll = coll;
		comp->respown_pos = pos;
		comp->respown_rotate = quaternion_identity();

	}

	//stage_parts.emplace_back(object);
	if (pearent != nullptr)pearent->add_child(object);
	return coll;
}

Collider* Stage_base::set_meshbox(const Vector3& pos, const Vector3& size, const Vector3& rotate, const Vector3& color, Gameobject* pearent, bool is_static ) {
	Gameobject* object = nullptr;
	object = Gameobject_manager::createFromFBX("", "./DefaultModel/cone.fbx");
	object->renderer->color = Vector4(color.x, color.y, color.z, 1);

	//object->addComponent<object_fall>();
	object->transform->local_orient = quaternion_from_euler(rotate);
	object->transform->local_pos = pos;
	object->transform->local_scale = size;

	//Box* M = object->addComponent<Box>();
	Collider* coll = object->addComponent<Collider>();
	coll->add_mesh_shape("./DefaultModel/cone.fbx");

	if (is_static) {
		coll->tag = Collider_tags::FBX | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Static_Stage ;
		coll->physics_data.is_moveable = false;
	}
	else {
		coll->tag = Collider_tags::FBX | Collider_tags::Stage | Collider_tags::Caera_not_sunk_Stage | Collider_tags::Kinematic_Stage ;
		// 動くのであれば所定のcomponentをアタッチする
		auto comp = object->addComponent<Kinematic_block>();
		comp->this_stage = this;
		comp->this_coll = coll;
		comp->respown_pos = pos;
		comp->respown_rotate = quaternion_identity();

	}

	//stage_parts.emplace_back(object);
	if (pearent != nullptr)pearent->add_child(object);
	return coll;
}

Gameobject* Stage_base::set_plane(const Vector3& pos, const Vector3& size, const Vector3& rotate, const Vector3& color, Gameobject* pearent, bool is_static ) {
	Gameobject* object = nullptr;
	object = Gameobject_manager::createPlane("plane");
	object->renderer->color = Vector4(color.x, color.y, color.z, 1);

	object->transform->local_orient = quaternion_from_euler(rotate);
	object->transform->local_pos = pos;
	object->transform->local_scale = size;

	//stage_parts.emplace_back(object);
	if (pearent != nullptr)pearent->add_child(object);
	return object;
}

Gameobject* Stage_base::set_respown_area(const Vector3& pos, const Vector3& size, const Vector3& rotate, int respown_num, Vector3 respown_pos, Gameobject* pearent) {

	auto go = Gameobject_manager::create("respown_area" + std::to_string(respown_num));
	go->transform->local_pos = pos;
	go->transform->local_scale = size;
	go->transform->local_orient = quaternion_from_euler(rotate);

	auto coll = go->addComponent<Collider>();
	coll->add_shape<Box>();
	coll->physics_data.is_hitable = false;
	coll->physics_data.is_moveable = false;

	auto comp = go->addComponent<Stage_parts::Area_respown_set>();
	comp->this_stage = this;
	comp->this_coll = coll;

	comp->respown_num = respown_num;
	comp->respown_position = respown_pos;

	if (pearent != nullptr)pearent->add_child(go);
	return go;
}

Gameobject* Stage_base::set_goal_area(const Vector3& pos, const Vector3& size, const Vector3& rotate, Stage_types next_stage, Gameobject* pearent) {

	auto go = Gameobject_manager::create("goal_area");
	go->transform->local_pos = pos;
	go->transform->local_scale = size;
	go->transform->local_orient = quaternion_from_euler(rotate);

	auto coll = go->addComponent<Collider>();
	coll->add_shape<Box>();
	coll->physics_data.is_hitable = false;
	coll->physics_data.is_moveable = false;

	auto comp = go->addComponent<Stage_parts::Area_goal>();
	comp->this_stage = this;
	comp->this_coll = coll;

	comp->next_stage = next_stage;

	if (pearent != nullptr)pearent->add_child(go);
	return go;
}

