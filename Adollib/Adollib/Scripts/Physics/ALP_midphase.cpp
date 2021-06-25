#include "ALP_midphase.h"
#include <list>
#include "../Imgui/work_meter.h"

#include "ALP_physics.h"
using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;
using namespace DOP;

//:::::::::::::::::::::::::::
#pragma region Midphase
//:::::::::::::::::::::::::::
//DOP14Ç…ÇÊÇÈëÂéGîcÇ»ìñÇΩÇËîªíË
bool Check_insert_DOP14(const Collider_shape* meshA, const Collider_shape* meshB) {
	//ñ≥å¿PlaneÇÕDOPÇ™çÏÇÍÇ»Ç¢ÇΩÇﬂnarrowÇ…ìäÇ∞ÇÈ?
	//if (meshA->shape == ALP_Collider_shape::Plane || meshB->shape == ALP_Collider_shape::Plane) return true;

	for (int i = 0; i < DOP14_size; i++) {
		const float dis = vector3_dot(DOP_14_axis[i], meshA->get_DOP().pos - meshB->get_DOP().pos);

		if (
			0 > +dis + meshA->get_DOP().max[i] - meshB->get_DOP().min[i] ||
			0 > -dis + meshB->get_DOP().max[i] - meshA->get_DOP().min[i]
			) {
			return false;
		}
	}

	return true;
}

bool Check_insert_Plane(const Collider_shape* plane, const Collider_shape* mesh) {

	Vector3 V;
	float plane_dis = 0, coll_dis = FLT_MAX;

	V = vector3_quatrotate(Vector3(0, 1, 0), plane->world_orientation());
	plane_dis = vector3_dot(V, plane->world_position());

	for (int i = 0; i < DOP14_size; i++) {
		float coll_len = vector3_dot(V, mesh->world_position() + DOP_14_axis[i] * mesh->get_DOP().max[i]);
		if (plane_dis > coll_len)return true;
	}

	return false;
}

void add_pair(std::vector<Contacts::Contact_pair>& pairs, Contacts::Contact_pair pair) {
	pairs.emplace_back(pair);
}

void Midphase_DOP_14(std::vector<Contacts::Contact_pair>& new_pairs, Collider_shape* meshA, Collider_shape* meshB) {
	Contact_pair new_pair;

	const ALP_Collider* collA = meshA->get_ALPcollider();
	const ALP_Collider* collB = meshB->get_ALPcollider();

	bool is_generate_contact = true;
	// sleepìØémÇÃèÍçáÇÕè’ìÀåüímÇÃÇ›
	//if (collA->get_ALPphysics()->is_sleep == true && collB->get_ALPphysics()->is_sleep == true)is_generate_contact = false;

	// É^ÉOÇ…ÇÊÇÈè’ìÀÇÃê•îÒ
	if (collA->get_ALPphysics()->is_hitable == false || (meshA->get_tag() & meshB->get_ignore_tags())) is_generate_contact = false;
	if (collB->get_ALPphysics()->is_hitable == false || (meshB->get_tag() & meshA->get_ignore_tags())) is_generate_contact = false;


	bool check_oncoll_only = false;
	if (is_generate_contact == false) {
		if (collA->get_oncoll_check_bits() & meshB->get_tag())check_oncoll_only = true;
		if (collB->get_oncoll_check_bits() & meshA->get_tag())check_oncoll_only = true;
	}
	if (is_generate_contact == false && check_oncoll_only == false)return;

	//DOPÇ…ÇÊÇÈëÂéGîcÇ»ìñÇΩÇËîªíË
	if (meshA->get_shape_tag() != ALPCollider_shape_type::Plane && meshB->get_shape_tag() != ALPCollider_shape_type::Plane) {
		if (Check_insert_DOP14(meshA, meshB) == false)return;
	}
	else if (meshA->get_shape_tag() == ALPCollider_shape_type::Plane && meshB->get_shape_tag() != ALPCollider_shape_type::Plane) {
		if (Check_insert_Plane(meshA, meshB) == false)return;
	}
	else if (meshA->get_shape_tag() != ALPCollider_shape_type::Plane && meshB->get_shape_tag() == ALPCollider_shape_type::Plane) {
		if (Check_insert_Plane(meshB, meshA) == false)return;
	}
	else return;

	//new_pair.body[0]Ç…ÉAÉhÉåÉXÇÃëÂÇ´Ç¢ÇŸÇ§ÇÇµÇ‹Ç§
	if (&*meshA > & *meshB) {
		new_pair.body[0] = meshA;
		new_pair.body[1] = meshB;
	}
	else {
		new_pair.body[0] = meshB;
		new_pair.body[1] = meshA;
	}
	new_pair.type = Pairtype::new_pair;
	new_pair.check_oncoll_only = check_oncoll_only;

	add_pair(new_pairs, new_pair);
}


void Physics_function::Midphase(std::vector<Contacts::Collider_2>& in_pair, std::vector<Contacts::Contact_pair>& old_pairs, std::vector<Contacts::Contact_pair>& new_pairs) {

	new_pairs.clear();

	Work_meter::start("Mid_Dop14");
	//DOP_14Ç…ÇÊÇÈëÂéGîcÇ»ìñÇΩÇËîªíË
	for (auto& pair : in_pair) {

		for (auto& meshB : pair.bodylists) {
			if (pair.body->get_ALPcollider() == meshB->get_ALPcollider())continue;

			Midphase_DOP_14(new_pairs, pair.body, meshB);
		}
	}
	Work_meter::stop("Mid_Dop14");


	const int old_pair_size = old_pairs.size();
	const int new_pair_size = new_pairs.size();

	Work_meter::start("Contact_pair_quick_sort");
	{
		if (new_pair_size != 0) {
			std::vector<Contacts::Contact_pair> pair_buff;
			pair_buff.resize(new_pair_size);
			Contact_pair_quick_sort(&new_pairs[0], &pair_buff[0], new_pair_size);
		}
	}
	Work_meter::stop("Contact_pair_quick_sort");

	//TODO : èdÇ¢ÅB óvåyó âª
	Work_meter::start("Mid_check_alive");
	{
		int oldId = 0, newId = 0;
		while (oldId < old_pair_size && newId < new_pair_size)
		{
			if (new_pairs[newId].key > old_pairs[oldId].key) {
				//oldÇÃkeyÇ™newÇÊÇËè¨Ç≥Ç¢ -> ÇªÇÃoldÇÕë∂ç›ÇµÇ»Ç¢
				oldId++;
			}
			else if (new_pairs[newId].key == old_pairs[oldId].key) {
				//oldÇÃkeyÇ∆newÇ™ìØÇ∂ -> ëOÇ©ÇÁë∂ç›ÇµÇƒÇ¢ÇÈ
				// keep
				new_pairs[newId] = old_pairs[oldId];
				new_pairs[newId].type = Contacts::Pairtype::keep_pair;
				oldId++;
				newId++;
			}
			else {
				//oldÇÃkeyÇ™newÇÊÇËëÂÇ´Ç¢ -> êVÇµÇ¢new
				// new
				new_pairs[newId].type = Contacts::Pairtype::new_pair;
				new_pairs[newId].contacts.reset();
				newId++;
			}

		}

	}

	Work_meter::stop("Mid_check_alive");

	Work_meter::start("Mid_remove_contact_point");
	//åªç›égópÇµÇƒÇ¢Ç»Ç¢è’ìÀì_ÇçÌèú
	for (auto& new_p : new_pairs) {
		new_p.contacts.chack_remove_contact_point(
			new_p.body[0],
			new_p.body[1]
		);
	}

	Work_meter::stop("Mid_remove_contact_point");
}
#pragma endregion
//:::::::::::::::::::::::::::