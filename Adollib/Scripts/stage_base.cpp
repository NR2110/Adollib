#include "stage_base.h"

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Physics/joint.h"

#include "buttan.h"
#include "door.h"
#include "kinematic_block.h"

using namespace Adollib;
using namespace Stage_parts;


Collider* Stage_base::set_sphere(Vector3 pos, float r, Vector3 color, bool is_static ) {

	Gameobject* object = nullptr;
	object = Gameobject_manager::createSphere(GO_tag::Sphere);

	object->material->color = Vector4(color.x, color.y, color.z, 1);
	object->transform->local_pos = pos;
	object->transform->local_scale = Vector3(r, r, r);

	Collider* coll = object->addComponent<Collider>();
	coll->add_shape<Sphere>();

	if (is_static) {
		coll->tag = Collider_tags::Sphere | Collider_tags::Stage | Collider_tags::Static_Stage | Collider_tags::Jumpable_Stage;
		coll->physics_data.is_moveable = false;
	}
	else {
		coll->tag = Collider_tags::Sphere | Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Jumpable_Stage;
		// 動くのであれば所定のcomponentをアタッチする
		auto comp = object->addComponent<Kinematic_block>();
		comp->this_stage = this;
		comp->this_coll = coll;
		comp->respown_pos = pos;
		comp->respown_rotate = quaternion_identity();

	}

	return coll;
}

Collider* Stage_base::set_box(Vector3 pos, Vector3 size, Vector3 rotate, Vector3 color, bool is_static ) {
	Gameobject* object = nullptr;
	object = Gameobject_manager::createCube(GO_tag::Box);
	object->material->color = Vector4(color.x, color.y, color.z, 1);

	object->transform->local_orient = quaternion_from_euler(rotate);
	object->transform->local_pos = pos;
	object->transform->local_scale = size;

	Collider* coll = object->addComponent<Collider>();
	coll->add_shape<Box>();

	if (is_static) {
		coll->tag = Collider_tags::Box | Collider_tags::Stage | Collider_tags::Static_Stage | Collider_tags::Jumpable_Stage;
		coll->physics_data.is_moveable = false;
	}
	else {
		coll->tag = Collider_tags::Box | Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Jumpable_Stage;
		// 動くのであれば所定のcomponentをアタッチする
		auto comp = object->addComponent<Kinematic_block>();
		comp->this_stage = this;
		comp->this_coll = coll;
		comp->respown_pos = pos;
		comp->respown_rotate = quaternion_identity();

	}

	return coll;
}

Collider* Stage_base::set_capsule(Vector3 pos, float r, float length, Vector3 rotate, Vector3 color, bool is_static ) {
	Gameobject* object = nullptr;
	object = Gameobject_manager::createCube(GO_tag::Capsule);
	object->material->color = Vector4(color.x, color.y, color.z, 1);

	object->transform->local_orient = quaternion_from_euler(rotate);
	object->transform->local_pos = pos;
	object->transform->local_scale = Vector3(r, r + length, r);

	Collider* coll = object->addComponent<Collider>();
	Capsule* capsule = coll->add_shape<Capsule>();
	capsule->length = ALmax(0, capsule->length - r);

	if (is_static) {
		coll->tag = Collider_tags::Capsule | Collider_tags::Stage | Collider_tags::Static_Stage | Collider_tags::Jumpable_Stage;
		coll->physics_data.is_moveable = false;
	}
	else {
		coll->tag = Collider_tags::Capsule | Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Jumpable_Stage;
		// 動くのであれば所定のcomponentをアタッチする
		auto comp = object->addComponent<Kinematic_block>();
		comp->this_stage = this;
		comp->this_coll = coll;
		comp->respown_pos = pos;
		comp->respown_rotate = quaternion_identity();

	}

	return coll;
}

Collider* Stage_base::set_meshbox(Vector3 pos, Vector3 size, Vector3 rotate, Vector3 color, bool is_static ) {
	Gameobject* object = nullptr;
	object = Gameobject_manager::createFromFBX("./DefaultModel/cone.fbx");
	object->material->color = Vector4(color.x, color.y, color.z, 1);

	//object->addComponent<object_fall>();
	object->transform->local_orient = quaternion_from_euler(rotate);
	object->transform->local_pos = pos;
	object->transform->local_scale = size;

	//Box* M = object->addComponent<Box>();
	Collider* coll = object->addComponent<Collider>();
	coll->add_shape("./DefaultModel/cone.fbx");

	if (is_static) {
		coll->tag = Collider_tags::FBX | Collider_tags::Stage | Collider_tags::Static_Stage | Collider_tags::Jumpable_Stage;
		coll->physics_data.is_moveable = false;
	}
	else {
		coll->tag = Collider_tags::FBX | Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Jumpable_Stage;
		// 動くのであれば所定のcomponentをアタッチする
		auto comp = object->addComponent<Kinematic_block>();
		comp->this_stage = this;
		comp->this_coll = coll;
		comp->respown_pos = pos;
		comp->respown_rotate = quaternion_identity();

	}

	return coll;
}

Collider* Stage_base::set_plane(Vector3 pos, Vector3 normal, Vector3 color, bool is_static ) {
	Gameobject* object = nullptr;
	object = Gameobject_manager::createCube(GO_tag::Plane);
	Vector4 C = Vector4(color.x, color.y, color.z, 1);
	object->material->color = C;

	object->transform->local_orient = quaternion_from_euler(normal);
	object->transform->local_pos = pos;
	object->transform->local_scale = Vector3(30, 0.1f, 30);

	Collider* coll = object->addComponent<Collider>();
	Plane* plane = coll->add_shape<Plane>();

	if (is_static) {
		coll->tag = Collider_tags::Plane | Collider_tags::Stage | Collider_tags::Static_Stage | Collider_tags::Jumpable_Stage;
		coll->physics_data.is_moveable = false;
	}

	return coll;
}

