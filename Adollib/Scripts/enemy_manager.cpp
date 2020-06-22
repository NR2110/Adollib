
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "enemy_manager.h"
#include "object_fall.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Enemy_manager::awake() {

	}

	void Enemy_manager::start()
	{
		Al_Global::Coefficient_of_friction = 0.98;
		Al_Global::resituation = 0.1;

		add_enemy(Enemy::Enemy_type::pat_1, vector3(10, 0, 5), quaternion_from_euler(0, 180, 0), vector4(1, 0, 0, 1));

		add_enemy(Enemy::Enemy_type::pat_2, vector3(0, 0, 10), quaternion_from_euler(0, 180, 0), vector4(0.8, 0, 0, 1));

		add_enemy(Enemy::Enemy_type::pat_3, vector3(-10, 0, 5), quaternion_from_euler(0, 180, 0), vector4(0.6, 0, 0, 1));

	}




	// 毎フレーム呼ばれる更新処理
	void Enemy_manager::update()
	{


	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Enemy_manager::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Enemy_manager::onDisable()
	{

	}

}

namespace Adollib {
	void Enemy_manager::add_enemy(Enemy::Enemy_type enemy_type, vector3 start_pos, quaternion start_orient,vector4 color) {

		Gameobject* GO = Gameobject_manager::createFromFBX("../Data/FBX/tank.fbx");
		Enemy* E = GO->addComponent<Enemy>();
		E->enemy_type = enemy_type;
		E->transform->local_pos = start_pos;
		E->transform->local_orient = start_orient;
		E->transform->local_scale = vector3(5,5,5);

		GO->material->color = color;

		object_fall* F = GO->addComponent<object_fall>();

		Collider* R =  GO->add_collider_box(vector3(0, 0.34, 0), vector3(0.50, 0.34, 0.72));
		E->collier = R;
		F->collier = R;

		enemys.push_back(E);

	}

}
