
#include "../Adollib/Adollib.h"
#include "../Adollib/gameobject_manager.h"

#include "enemy_manager.h"
#include "object_fall.h"

namespace Adollib
{
	// ��������V�[���̏��������Ɉ�x�����Ă΂��
	void Enemy_manager::awake() {

	}

	void Enemy_manager::start()
	{
		Al_Global::Coefficient_of_friction = 0.6;
		Al_Global::resituation = 0.4;

		add_enemy(Enemy::Enemy_type::normal, vector3(5, 0, 5), quaternion_from_euler(0,180,0));
		//add_enemy(Enemy::Enemy_type::normal, vector3(5, 5, 5.06), quaternion_identity());
	}




	// ���t���[���Ă΂��X�V����
	void Enemy_manager::update()
	{


	}

	// ���t���[���Aupdate()��ɌĂ΂��X�V����
	void Enemy_manager::lateUpdate()
	{


	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
	void Enemy_manager::onEnable()
	{

	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
	void Enemy_manager::onDisable()
	{

	}

}

namespace Adollib {
	void Enemy_manager::add_enemy(Enemy::Enemy_type enemy_type, vector3 start_pos, quaternion start_orient) {

		Gameobject* GO = Gameobject_manager::createFromFBX("../Data/FBX/tank.fbx");
		Enemy* E = GO->addComponent<Enemy>();
		E->enemy_type = enemy_type;
		E->transform->local_pos = start_pos;
		E->transform->local_orient = start_orient;
		E->transform->local_scale = vector3(5,5,5);

		object_fall* F = GO->addComponent<object_fall>();

		Rigitbody* R =  GO->add_collider_box(vector3(0, 0.34, 0), vector3(0.50, 0.34, 0.72));
		E->collier = R;
		F->collier = R;

		enemys.push_back(E);

	}

}
