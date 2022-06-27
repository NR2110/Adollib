
#include "collider_comp.h"

#include <Adollibphysics.h>
#include "../../Scripts/Object/gameobject.h"

using namespace Adollib;

void Collider_comp::awake() {
	coll = std::make_shared<Collider>();
	auto p_orient_inv = Quaternion(1, 0, 0, 0);
	if (gameobject->parent() != nullptr)p_orient_inv = gameobject->parent()->transform->orientation.inverse();
	coll->awake(gameobject, gameobject->world_position(), gameobject->world_orientate(), gameobject->world_scale(), p_orient_inv);
}

void Collider_comp::update() {
	//coll->Wposition = transform->position;
	//coll->Worientation = transform->orientation;
	//coll->Wscale = transform->scale;

	//auto p_orient_inv = Quaternion(1, 0, 0, 0);
	//if (gameobject->parent() != nullptr)p_orient_inv = gameobject->parent()->transform->orientation.inverse();
	//coll->pearent_Worientation_inverse = p_orient_inv;

	//Vector3 pos_change;
	//Quaternion orient_change;
	//coll->update_get_amount_of_change(pos_change, orient_change);

	//transform->local_pos += pos_change;
	//transform->local_orient *= orient_change;

	//coll->update_set_Wtransform(transform->position + pos_change, transform->orientation * orient_change, transform->scale, p_orient_inv);

	coll->update();

	Vector3 pearent_Wpos = Vector3(0);
	Quaternion pearent_Worient = Quaternion(1, 0, 0, 0);
	Vector3 pearent_Wscale = Vector3(1);

	if (gameobject->parent() != nullptr) {
		pearent_Wpos = gameobject->parent()->transform->position;
		pearent_Worient = gameobject->parent()->transform->orientation;
		pearent_Wscale = gameobject->parent()->transform->scale;
	}

	const Vector3& coll_Wpos = coll->Wposition;
	const Quaternion& coll_Worient = coll->Worientation;

	transform->local_pos = vector3_quatrotate(coll_Wpos - pearent_Wpos, pearent_Worient.inverse()) / pearent_Wscale;
	transform->local_orient = coll_Worient * pearent_Worient.inverse();


}

void Collider_comp::finalize() {
	coll->finalize();
}

void Collider_comp::Update_hierarchy() {

}