
#include "collobject_creater.h"

#include "../../Scripts/Object/gameobject_manager.h"
#include "../../Scripts/Renderer/renderer_base.h"

#include <Adollibphysics.h>

using namespace Adollib;

Collider_comp* Collider_creater::create_box(const std::string& name, const Vector3& pos, const Vector3& orient, const Vector3 scale, const Vector3& color, Gameobject* pearent) {

	auto go = Gameobject_manager::createCube(name);
	go->transform->local_pos = pos;
	go->transform->local_orient = quaternion_from_euler(orient);
	go->transform->local_scale = scale;
	go->renderer->color = Vector4(color / 255,1);

	if (pearent != nullptr) pearent->add_child(go);

	auto comp = go->addComponent<Collider_comp>();
	comp->get_collider()->add_shape<Box>();
	return comp;

}

Collider_comp* Collider_creater::create_sphere(const std::string& name, const Vector3& pos, float r, const Vector3& color, Gameobject* pearent) {

	auto go = Gameobject_manager::createSphere(name);
	go->transform->local_pos = pos;
	go->transform->local_scale = Vector3(r);
	go->renderer->color = Vector4(color / 255, 1);

	if (pearent != nullptr) pearent->add_child(go);

	auto comp = go->addComponent<Collider_comp>();
	comp->get_collider()->add_shape<Sphere>();
	return comp;

}

Collider_comp* Collider_creater::create_capsule(const std::string& name, const Vector3& pos, float length, float r, const Vector3& color, Gameobject* pearent) {

	auto go = Gameobject_manager::createCylinder(name);
	go->transform->local_pos = pos;
	go->transform->local_scale = Vector3(r, length, r);
	go->renderer->color = Vector4(color / 255, 1);

	{
		auto child = Gameobject_manager::createSphere(name);
		child->transform->local_pos = Vector3(0, +length, 0) / go->transform->local_scale;
		child->transform->local_scale = Vector3(r);
		child->renderer->color = Vector4(color / 255, 1);
		go->add_child(child);
	}
	{
		auto child = Gameobject_manager::createSphere(name);
		child->transform->local_pos = Vector3(0, -length, 0) / go->transform->local_scale;
		child->transform->local_scale = Vector3(r);
		child->renderer->color = Vector4(color / 255, 1);
		go->add_child(child);
	}

	if (pearent != nullptr) pearent->add_child(go);

	auto comp = go->addComponent<Collider_comp>();
	auto shape = comp->get_collider()->add_shape<Capsule>();
	return comp;

}