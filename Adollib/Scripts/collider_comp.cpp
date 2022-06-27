
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

	const Vector3 save_Wpos = coll->Wposition;
	const Quaternion save_Worient = coll->Worientation;
	coll->update();
	const Vector3& coll_Wpos = coll->Wposition;
	const Quaternion& coll_Worient = coll->Worientation;

	Vector3 pearent_Wpos = Vector3(0);
	Quaternion pearent_Worient = Quaternion(1, 0, 0, 0);
	Vector3 pearent_Wscale = Vector3(1);

	if (gameobject->parent() != nullptr) {
		pearent_Wpos = gameobject->parent()->transform->position;
		pearent_Worient = gameobject->parent()->transform->orientation;
		pearent_Wscale = gameobject->parent()->transform->scale;
	}

	const Vector3 pos_amount_of_change       = coll_Wpos - save_Wpos;
	const Quaternion orient_amount_of_change = (save_Worient * pearent_Worient.inverse()).inverse() * coll_Worient * pearent_Worient.inverse();

	transform->local_pos += vector3_quatrotate(pos_amount_of_change, pearent_Worient.inverse()) / pearent_Wscale;
	transform->local_orient *= orient_amount_of_change;

	gameobject->update_world_trans_to_children();

	coll->update_Wtransform(gameobject->world_position(), gameobject->world_orientate(), gameobject->world_scale(), pearent_Worient.inverse());

}

void Collider_comp::finalize() {
	coll->finalize();
}

void Collider_comp::Update_hierarchy() {

}