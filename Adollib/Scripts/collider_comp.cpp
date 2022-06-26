
#include "collider_comp.h"

#include "../../Scripts/Object/gameobject.h"

using namespace Adollib;

void Collider_comp::awake() {
	coll = std::make_shared<Collider>();
	auto p_orient_inv = Quaternion(1, 0, 0, 0);
	if (gameobject->parent() != nullptr)p_orient_inv = gameobject->parent()->transform->orientation.inverse();
	coll->awake(gameobject, transform->position, transform->orientation, transform->scale, p_orient_inv);
}

void Collider_comp::update() {
	//coll->Wposition = transform->position;
	//coll->Worientation = transform->orientation;
	//coll->Wscale = transform->scale;
	auto p_orient_inv = Quaternion(1, 0, 0, 0);
	if (gameobject->parent() != nullptr)p_orient_inv = gameobject->parent()->transform->orientation.inverse();
	coll->pearent_Worientation_inverse = p_orient_inv;

	Vector3 pos_change;
	Quaternion orient_change;
	coll->update_get_amount_of_change(pos_change, orient_change);


	transform->local_pos += pos_change;
	transform->local_orient *= orient_change;

	coll->update_set_Wtransform(transform->position + pos_change, transform->orientation * orient_change, transform->scale, p_orient_inv);

}

void Collider_comp::finalize() {
	coll->finalize();
}

void Collider_comp::Update_hierarchy() {

}