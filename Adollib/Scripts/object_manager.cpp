
#include "../Adollib/gameobject_manager.h"

#include "object_manager.h"
#include "object_fall.h"

#include "../Adollib/closest_func.h"
namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void object_manager::awake() {

	}

	void object_manager::start()
	{
#if 1
		//set_sphere(n_vector3(0, 0, 50), 10);
		//set_sphere(n_vector3(0, 0, 0), 10);
		//set_moveable_sphere(n_vector3(0, 30, 0), 5);
		//set_moveable_sphere(n_vector3(0, 50, 0), 0.1);

		//set_plane(vector3(0, 0, 0), vector3(0, 1, 0), vector3(1, 0, 1));
		//	set_nohit_plane(n_vector3(1000, -1, 1000), n_vector3(0, 1, 0), n_vector3(1, 0, 1));
			//set_moveable_box(n_vector3(0, 200, 0), n_vector3(10, 10, 10), n_vector3(45, 45, 0), n_vector3(0, 1, 1));
			//set_moveable_box(n_vector3(0, 200, 0), n_vector3(10, 10, 10), n_vector3(0, 0, 0), n_vector3(0, 1, 1));
		{
			Gameobject* GO = Gameobject_manager::createCube();
			GO->transform->local_pos = vector3(0, -60, 0);
			GO->transform->local_scale = vector3(60, 60, 60);
			GO->material->color = vector4(1, 1, 1, 1);

			Collider* R = GO->add_collider_box();
			R->move = false;
		}
		//{
		//	Gameobject* GO = Gameobject_manager::createCube();
		//	GO->transform->local_pos = vector3(0, -35, 0);
		//	GO->transform->local_orient = quaternion_from_euler(0, 0, 45);
		//	GO->transform->local_scale = vector3(30, 30, 30);
		//	GO->material->color = vector4(1, 1, 1, 1);

		//	Collider* R = GO->add_collider_box();
		//	R->move = false;
		//}

		//set_fall_box(vector3(0, 4, 0), vector3(2, 2, 2), vector3(0, 0, 0), vector3(0, 1, 1));
		//set_fall_box(vector3(0, 8, 0), vector3(2, 2, 2), vector3(0, 0, 0), vector3(0, 1, 1));

		for (int i = 0; i < 2; i++) {
			//set_fall_box(vector3(0, 5 + 10 * i, 0), vector3(5,2,5), vector3(0,i*45,0), vector3(0, 1, 1));
			//set_fall_box(vector3(5, 5 + 10 * i, 0), vector3(2,2,2), vector3(0,i*45,0), vector3(0, 1, 1));
			//set_fall_box(vector3(5, 5 + 10 * i, 5), vector3(2,2,2), vector3(0,i*45,0), vector3(0, 1, 1));
			//set_fall_box(vector3(0, 5 + 10 * i, 5), vector3(2,2,2), vector3(0,i*45,0), vector3(0, 1, 1));

			//set_fall_sphere(vector3(i * 0.01, 20 + 10 * i, i * 0.01), 2, vector3(0, 1, 1));
			//set_fall_sphere(vector3(5, 5 + 10 * i, i * 0.01), 2, vector3(0, 1, 1));
			//set_fall_sphere(vector3(5, 5 + 10 * i, 5), 2, vector3(0, 1, 1));
			//set_fall_sphere(vector3(i * 0.01, 20 + 10 * i, 5), 2, vector3(0, 1, 1));

			//set_fall_sphere(vector3(0, 5 + 10 * i, 0), 2, vector3(0, 1, 1));
			//set_fall_sphere(vector3(5, 5 + 10 * i, 0), 2, vector3(0, 1, 1));
			//set_fall_sphere(vector3(5, 5 + 10 * i, 5), 2, vector3(0, 1, 1));
			//set_fall_sphere(vector3(0, 5 + 10 * i, 5), 2, vector3(0, 1, 1));


		}

		//int max_c = 10;
		//for (int i = 0; i < max_c; i++) {

		//	for (int o = 0; o < max_c - i; o++) {
		//		//set_fall_box(vector3(2.00001 * o - (max_c - i) * 2.000001 / 2.0, 1.0 + 2.00001 * i, 0), vector3(1, 1, 1), vector3(0, 0, 0), vector3(0, 1, 1));
		//		Box* V = set_fall_box(vector3(2.50001 * o - (max_c - i) * 2.500001 / 2.0, 5.0 + 2.50001 * i, 0), vector3(1, 1, 1), vector3(30, 0, 30), vector3(0, 1, 1));
		//		boxes.emplace_back(V);
		//		st_box.emplace_back(*V);
		//	}

		//	//set_fall_sphere(vector3(i * 0.01, 20 + 10 * i, i * 0.01), 2, vector3(0, 1, 1));
		//	//set_fall_sphere(vector3(5, 5 + 10 * i, i * 0.01), 2, vector3(0, 1, 1));
		//	//set_fall_sphere(vector3(5, 5 + 10 * i, 5), 2, vector3(0, 1, 1));
		//	//set_fall_sphere(vector3(i * 0.01, 20 + 10 * i, 5), 2, vector3(0, 1, 1));

		//	//set_fall_sphere(vector3(0, 5 + 10 * i, 0), 2, vector3(0, 1, 1));
		//	//set_fall_sphere(vector3(5, 5 + 10 * i, 0), 2, vector3(0, 1, 1));
		//	//set_fall_sphere(vector3(5, 5 + 10 * i, 5), 2, vector3(0, 1, 1));
		//	//set_fall_sphere(vector3(0, 5 + 10 * i, 5), 2, vector3(0, 1, 1));

		//}

		//set_fall_box(vector3(0, 20, 0), vector3(3, 3, 3), vector3(45, 0, 0), vector3(0, 1, 1));

		//Box* B = set_fall_box(vector3(0, 0.25, 0), vector3(2, 0.25, 1), vector3(0, 90, 0), vector3(0, 0.5, 1));
		//B->move = false;

		//set_fall_box(vector3(0, 3, 0), vector3(2, 0.25, 1), vector3(0, 0, 45), vector3(0, 1, 1));

#else
		//	set_plane(n_vector3(0, -1, 0), n_vector3(0, 1, 0), n_vector3(1, 0, 1));
		set_box(n_vector3(0, 0, 5), n_vector3(1, 1, 1), n_vector3(0, 0, 0));
#endif

		//	set_moveable_box(n_vector3(1000, 100, 1000), n_vector3(2, 2, 2), n_vector3(30, 30, 0), n_vector3(0, 1, 1));

	}

	// 毎フレーム呼ばれる更新処理
	void object_manager::update()
	{
		if (input->getKeyTrigger(Key::M)) {
			int max_c = 10;
			for (int i = 0; i < max_c; i++) {

				for (int o = 0; o < max_c - i; o++) {
					set_fall_box(vector3(2.00001 * o - (max_c - i) * 2.000001 / 2.0, 1.0 + 2.00001 * i, 0), vector3(1, 1, 1), vector3(0, 0, 0), vector3(0, 1, 1));
					//Box* V = set_fall_box(vector3(2.50001 * o - (max_c - i) * 2.500001 / 2.0, 5.0 + 2.50001 * i, 0), vector3(1, 1, 1), vector3(30, 0, 30), vector3(0, 1, 1));
					//boxes.emplace_back(V);
					//st_box.emplace_back(*V);
				}

				//set_fall_sphere(vector3(i * 0.01, 20 + 10 * i, i * 0.01), 2, vector3(0, 1, 1));
				//set_fall_sphere(vector3(5, 5 + 10 * i, i * 0.01), 2, vector3(0, 1, 1));
				//set_fall_sphere(vector3(5, 5 + 10 * i, 5), 2, vector3(0, 1, 1));
				//set_fall_sphere(vector3(i * 0.01, 20 + 10 * i, 5), 2, vector3(0, 1, 1));

				//set_fall_sphere(vector3(0, 5 + 10 * i, 0), 2, vector3(0, 1, 1));
				//set_fall_sphere(vector3(5, 5 + 10 * i, 0), 2, vector3(0, 1, 1));
				//set_fall_sphere(vector3(5, 5 + 10 * i, 5), 2, vector3(0, 1, 1));
				//set_fall_sphere(vector3(0, 5 + 10 * i, 5), 2, vector3(0, 1, 1));

			}
		}

	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void object_manager::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void object_manager::onDisable()
	{

	}

}

namespace Adollib
{
	Sphere* object_manager::set_sphere(vector3 pos, float r, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(r, r, r);

		return object->add_collider_sphere();
	}

	Box* object_manager::set_box(vector3 pos, vector3 size, vector3 rotate, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		return object->add_collider_box();
	}

	Plane* object_manager::set_plane(vector3 pos, vector3 normal, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(normal);
		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(30, 0.1, 30);

		return object->add_collider_plane(vector3(0, 0, 0), normal);
	}

	//
	Sphere* object_manager::set_fall_sphere(vector3 pos, float r, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(r, r, r);

		Sphere* S = object->add_collider_sphere();

		object_fall* F = object->addComponent<object_fall>();
		F->collier = object->collider[0].get();

		return S;
	}

	Box* object_manager::set_fall_box(vector3 pos, vector3 size, vector3 rotate, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		Box* B = object->add_collider_box();
		B->inertial_mass = 1;

		object_fall* F = object->addComponent<object_fall>();
		F->collier = object->collider[0].get();

		return B;
	}

	//Gameobject* object_manager::set_nohit_plane(vector3 pos, vector3 normal, vector3 color) {
	//	Gameobject* object = nullptr;
	//	object = Gameobject_manager::createCube("");
	//	vector4 C = vector4(color.x, color.y, color.z, 1);
	//	object->material->color = C;

	//	object->transform->local_orient = quaternion_from_euler(normal);
	//	object->transform->local_pos = pos;
	//	object->transform->local_scale = vector3(30, 30, 0.1);

	//	return object;
	//}
	Gameobject* object_manager::set_nohit_sphere(vector3 pos, float r, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(r, r, r);

		return object;
	}

	Gameobject* object_manager::set_nohit_box(vector3 pos, vector3 size, vector3 rotate, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		return object;
	}

	Gameobject* object_manager::set_nohit_plane(vector3 pos, vector3 normal, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(normal);
		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(30, 30, 0.1);

		std::vector<std::string> no;
		no.push_back("all");
		object->add_collider_plane(pos, normal, 1, "plane", no);

		return object;
	}


}