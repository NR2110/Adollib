#include "ALP_midphase.h"
#include <list>

#include "ALP_physics.h"
using namespace Adollib;
using namespace Physics_function;
using namespace Contacts;
using namespace DOP;


//:::::::::::::::::::::::::::
#pragma region Midphase
//:::::::::::::::::::::::::::

// Broadphase‚É‚¨ˆø‰z‚µ‚µ‚Ü‚µ‚½
////DOP14‚É‚æ‚é‘åŽG”c‚È“–‚½‚è”»’è
//bool Check_insert_DOP14(const Collider_shape* meshA, const Collider_shape* meshB) {
//	//–³ŒÀPlane‚ÍDOP‚ªì‚ê‚È‚¢‚½‚ßnarrow‚É“Š‚°‚é?
//	//if (meshA->shape == ALP_Collider_shape::Plane || meshB->shape == ALP_Collider_shape::Plane) return true;
//
//	for (int i = 0; i < DOP14_size; i++) {
//		const float dis = vector3_dot(DOP_14_axis[i], meshA->get_DOP().pos - meshB->get_DOP().pos);
//
//		if (
//			0 > +dis + meshA->get_DOP().max[i] - meshB->get_DOP().min[i] ||
//			0 > -dis + meshB->get_DOP().max[i] - meshA->get_DOP().min[i]
//			) {
//			return false;
//		}
//	}
//
//	return true;
//}
//
//bool Check_insert_Plane(const Collider_shape* plane, const Collider_shape* mesh) {
//
//	Vector3 V;
//	float plane_dis = 0, coll_dis = FLT_MAX;
//
//	V = vector3_quatrotate(Vector3(0, 1, 0), plane->world_orientation());
//	plane_dis = vector3_dot(V, plane->world_position());
//
//	for (int i = 0; i < DOP14_size; i++) {
//		float coll_len = vector3_dot(V, mesh->world_position() + DOP_14_axis[i] * mesh->get_DOP().max[i]);
//		if (plane_dis > coll_len)return true;
//	}
//
//	return false;
//}
//
//void add_pair(std::vector<Contacts::Contact_pair>& pairs, Contacts::Contact_pair pair) {
//	pairs.emplace_back(pair);
//}
//
//void Midphase_DOP_14(std::vector<Contacts::Contact_pair>& new_pairs, Collider_shape* meshA, Collider_shape* meshB) {
//	Contact_pair new_pair;
//
//	const ALP_Collider* collA = meshA->get_ALPcollider();
//	const ALP_Collider* collB = meshB->get_ALPcollider();
//
//	bool is_generate_contact = true;
//	// sleep“¯Žm‚Ìê‡‚ÍÕ“ËŒŸ’m‚Ì‚Ý
//	//if (collA->get_ALPphysics()->is_sleep == true && collB->get_ALPphysics()->is_sleep == true)is_generate_contact = false;
//
//	// ƒ^ƒO‚É‚æ‚éÕ“Ë‚Ì¥”ñ
//	if (collA->get_ALPphysics()->is_hitable == false || (meshA->get_tag() & meshB->get_ignore_tags())) is_generate_contact = false;
//	if (collB->get_ALPphysics()->is_hitable == false || (meshB->get_tag() & meshA->get_ignore_tags())) is_generate_contact = false;
//
//
//	bool check_oncoll_only = false;
//	if (is_generate_contact == false) {
//		if (collA->get_oncoll_check_bits() & meshB->get_tag())check_oncoll_only = true;
//		if (collB->get_oncoll_check_bits() & meshA->get_tag())check_oncoll_only = true;
//	}
//	if (is_generate_contact == false && check_oncoll_only == false)return;
//
//	//DOP‚É‚æ‚é‘åŽG”c‚È“–‚½‚è”»’è
//	if (meshA->get_shape_tag() != ALPCollider_shape_type::Plane && meshB->get_shape_tag() != ALPCollider_shape_type::Plane) {
//		if (Check_insert_DOP14(meshA, meshB) == false)return;
//	}
//	else if (meshA->get_shape_tag() == ALPCollider_shape_type::Plane && meshB->get_shape_tag() != ALPCollider_shape_type::Plane) {
//		if (Check_insert_Plane(meshA, meshB) == false)return;
//	}
//	else if (meshA->get_shape_tag() != ALPCollider_shape_type::Plane && meshB->get_shape_tag() == ALPCollider_shape_type::Plane) {
//		if (Check_insert_Plane(meshB, meshA) == false)return;
//	}
//	else return;
//
//	//new_pair.body[0]‚ÉƒAƒhƒŒƒX‚Ì‘å‚«‚¢‚Ù‚¤‚ð‚µ‚Ü‚¤
//	if (&*meshA > & *meshB) {
//		new_pair.body[0] = meshA;
//		new_pair.body[1] = meshB;
//	}
//	else {
//		new_pair.body[0] = meshB;
//		new_pair.body[1] = meshA;
//	}
//	new_pair.type = Pairtype::new_pair;
//	new_pair.check_oncoll_only = check_oncoll_only;
//
//	add_pair(new_pairs, new_pair);
//}


void Physics_function::Midphase(std::vector<Contacts::Contact_pair*>& old_pairs, std::vector<Contacts::Contact_pair*>& new_pairs) {

	//new_pairs.clear();


	////DOP_14‚É‚æ‚é‘åŽG”c‚È“–‚½‚è”»’è
	// Broadphase‚É‚¨ˆø‰z‚µ‚µ‚Ü‚µ‚½
	//for (auto& pair : in_pair) {

	//	for (auto& meshB : pair.bodylists) {
	//		if (pair.body->get_ALPcollider() == meshB->get_ALPcollider())continue;

	//		Midphase_DOP_14(new_pairs, pair.body, meshB);
	//	}
	//}



	const int old_pair_size = old_pairs.size();
	const int new_pair_size = new_pairs.size();


	{

		if (new_pair_size != 0) {

		//Work_meter::start("save_pointer_quick_sort");
		//	std::vector<Contacts::Contact_pair*> pair_buff;
		//	//pair_buff.reserve(new_pair_size);
		//	for (auto& n : new_pairs) {
		//		pair_buff.emplace_back(&n);
		//	}
		//Work_meter::stop("save_pointer_quick_sort");


			Contact_pair_quick_sort(new_pairs, 0, new_pair_size - 1);


			//Work_meter::start("copy_quick_sort");
			//std::vector<Contacts::Contact_pair> pair_buff_;
			//pair_buff_.resize(new_pair_size);
			//for (int i = 0; i < new_pair_size; i++) {
			//	pair_buff_[i] = *pair_buff[i];
			//}
			//new_pairs = pair_buff_;
			//Work_meter::stop("copy_quick_sort");

			//Contact_pair_quick_sort(new_pairs.data(), pair_buff.data(), new_pair_size);
		}
	}

	{
		int oldId = 0, newId = 0;
		while (oldId < old_pair_size && newId < new_pair_size)
		{
			if (new_pairs[newId]->key > old_pairs[oldId]->key) {
				//old‚Ìkey‚ªnew‚æ‚è¬‚³‚¢ -> ‚»‚Ìold‚Í‘¶Ý‚µ‚È‚¢
				delete old_pairs[oldId];
				oldId++;
			}
			else if (new_pairs[newId]->key == old_pairs[oldId]->key) {
				//old‚Ìkey‚Ænew‚ª“¯‚¶ -> ‘O‚©‚ç‘¶Ý‚µ‚Ä‚¢‚é
				// keep
				delete new_pairs[newId];
				new_pairs[newId] = old_pairs[oldId];
				new_pairs[newId]->type = Contacts::Pairtype::keep_pair;
				oldId++;
				newId++;
			}
			else {
				//old‚Ìkey‚ªnew‚æ‚è‘å‚«‚¢ -> V‚µ‚¢new
				// new
				new_pairs[newId]->type = Contacts::Pairtype::new_pair;
				new_pairs[newId]->contacts.reset();
				newId++;
			}

		}

		while (newId < new_pair_size) {
			// Žc‚è‚Í‘S•”new
			new_pairs[newId]->type = Contacts::Pairtype::new_pair;
			new_pairs[newId]->contacts.reset();
			newId++;
		}

		while (oldId < old_pair_size) {
			// Žc‚è‚Í‘S•”old
			delete old_pairs[oldId];
			oldId++;
		}

		old_pairs.clear();
	}

	//Œ»ÝŽg—p‚µ‚Ä‚¢‚È‚¢Õ“Ë“_‚ðíœ
	for (auto& new_p : new_pairs) {
		if (new_p->contacts.chack_remove_contact_point(
			new_p->body[0],
			new_p->body[1]
		)) {
			//new_p.body[0]->get_ALPcollider()->get_ALPphysics()->sleep_timer = 0.0f;
			//new_p.body[1]->get_ALPcollider()->get_ALPphysics()->sleep_timer = 0.0f;
		}
	}


}
#pragma endregion
//:::::::::::::::::::::::::::