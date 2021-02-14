#pragma once
#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"
#include "../Adollib/Scripts/Object/gameobject_camera.h"

namespace Adollib
{

	class object_manager : public Component
	{
	private:
		Gameobject* sphere_go;
		Gameobject* normal_go;

		Gameobject* objGO;

		std::vector<Gameobject*> GOs;
		std::vector <Collider*> boxes;

		Camera* camera;

		bool is_draw_raycast = true;

	public:
		Gameobject* set_sphere(Vector3 pos, float r, Vector3 color = Vector3(1, 1, 1));
		Gameobject* set_box(Vector3 pos, Vector3 size, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));
		Gameobject* set_capsule(Vector3 pos, float r,float length,Vector3 rotate, Vector3 color = Vector3(1, 1, 1));
		Gameobject* set_plane(Vector3 pos, Vector3 normal, Vector3 color = Vector3(1, 1, 1));

		Gameobject* set_meshbox(Vector3 pos, Vector3 size, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));

	public:
		void awake();

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start();

		// ���t���[���Ă΂��X�V����
		void update();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable();

	};

}