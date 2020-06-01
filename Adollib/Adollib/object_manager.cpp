
#include "gameobject_manager.h"

#include "object_manager.h"
#include "object_fall.h"
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

		set_plane(vector3(0, -1, 0), vector3(0, 1, 0), vector3(1, 0, 1));
		//	set_nohit_plane(n_vector3(1000, -1, 1000), n_vector3(0, 1, 0), n_vector3(1, 0, 1));
			//set_moveable_box(n_vector3(0, 200, 0), n_vector3(10, 10, 10), n_vector3(45, 45, 0), n_vector3(0, 1, 1));
			//set_moveable_box(n_vector3(0, 200, 0), n_vector3(10, 10, 10), n_vector3(0, 0, 0), n_vector3(0, 1, 1));

		//set_moveable_box(vector3(0, 5 + 10 * 0, 0), vector3(2, 2, 2), vector3(30, 30, 0), vector3(0, 1, 1));


		for (int i = 0; i < 10; i++) {
			set_fall_box(vector3(0, 5 + 10 * i, 0), vector3(2, 2, 2), vector3(30, 30, 0), vector3(0, 1, 1));
			set_fall_box(vector3(5, 5 + 10 * i, 0), vector3(2, 2, 2), vector3(30, 30, 0), vector3(0, 1, 1));
			set_fall_box(vector3(5, 5 + 10 * i, 5), vector3(2, 2, 2), vector3(30, 30, 0), vector3(0, 1, 1));
			set_fall_box(vector3(0, 5 + 10 * i, 5), vector3(2, 2, 2), vector3(30, 30, 0), vector3(0, 1, 1));

			//set_moveable_sphere(n_vector3(i, 5 + 10 * i, i), 2,			n_vector3(1, 1, 0));
			//set_moveable_sphere(n_vector3(i + 5, 5 + 10 * i, i), 2,		n_vector3(1, 1, 0));
			//set_moveable_sphere(n_vector3(i + 5, 5 + 10 * i, 5 + i), 2, n_vector3(1, 1, 0));
			//set_moveable_sphere(n_vector3(i, 5 + 10 * i, 5 + i), 2,		n_vector3(1, 1, 0));

		}

#else
		//	set_plane(n_vector3(0, -1, 0), n_vector3(0, 1, 0), n_vector3(1, 0, 1));
		set_box(n_vector3(0, 0, 5), n_vector3(1, 1, 1), n_vector3(0, 0, 0));
#endif

		//	set_moveable_box(n_vector3(1000, 100, 1000), n_vector3(2, 2, 2), n_vector3(30, 30, 0), n_vector3(0, 1, 1));

	}

	// 毎フレーム呼ばれる更新処理
	void object_manager::update()
	{
		//n_matrix A;
		//A._11 = 1;	A._12 = 0;	A._13 = 0;	A._14 = 100;
		//A._21 = 0;	A._22 = 1;	A._23 = 0;	A._24 = 0;
		//A._31 = 0;	A._32 = 0;	A._33 = 1;	A._34 = 0;
		//A._41 = 0;	A._42 = 0;	A._43 = 0;	A._44 = 1;

		//n_matrix B = matrix_inverse(A);

		//int dafsgdf =1;

	}
	// 毎フレーム、update()後に呼ばれる更新処理
	void object_manager::lateUpdate()
	{


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
	void object_manager::set_sphere(vector3 pos, float r, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(r, r, r);

		object->add_collider_sphere();
	}

	void object_manager::set_box(vector3 pos, vector3 size, vector3 rotate, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		object->add_collider_box();
	}

	void object_manager::set_plane(vector3 pos, vector3 normal, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		//object->addComponent<object_fall>();
		object->transform->local_orient = quaternion_from_euler(normal);
		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(30, 0.1, 30);

		object->add_collider_plane(vector3(0,0,0), normal);
	}

	//
	void object_manager::set_fall_sphere(vector3 pos, float r, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createSphere("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(r, r, r);

		object->add_collider_sphere();

		object_fall* F = object->addComponent<object_fall>();
		F->collier = object->collider[0];

	}

	void object_manager::set_fall_box(vector3 pos, vector3 size, vector3 rotate, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_orient = quaternion_from_euler(rotate);
		object->transform->local_pos = pos;
		object->transform->local_scale = size;

		object->add_collider_box();

		object_fall* F = object->addComponent<object_fall>();
		F->collier = object->collider[0];

	}

	void object_manager::set_fall_plane(vector3 pos, vector3 normal, vector3 color) {
		Gameobject* object = nullptr;
		object = Gameobject_manager::createCube("");
		vector4 C = vector4(color.x, color.y, color.z, 1);
		object->material->color = C;

		object->transform->local_orient = quaternion_from_euler(normal);
		object->transform->local_pos = pos;
		object->transform->local_scale = vector3(30, 30, 0.1);

		object->add_collider_plane(pos, normal);

		object_fall* F = object->addComponent<object_fall>();
		F->collier = object->collider[0];

	}

	void object_manager::set_nohit_plane(vector3 pos, vector3 normal, vector3 color) {
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

	}


}