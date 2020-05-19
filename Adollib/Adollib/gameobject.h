#pragma once

#include <list>
#include <memory>

#include "object.h"
#include "component.h"
#include "transform.h"
#include "scene_list.h"

namespace Adollib {

	class Gameobject : public object {
	public:
		std::string name = std::string("null"); //����go�̖��O(�����p)

		Transfome* transform = nullptr; //�`��p�̏��
		std::list < std::shared_ptr< RigitBody_Transform>> collider; //�A�^�b�`����Ă���collider

		std::list <std::shared_ptr<Component>> components; //�A�^�b�`����Ă���Conponent�̃|�C���^

		Gameobject* pearent = nullptr; //�e�ւ̃|�C���^
		std::list<std::shared_ptr<Gameobject>> children; //�ւ̃|�C���^

		bool active = true; //false�Ȃ�X�V�A�`����~�߂�

		Scenelist this_scene = Scenelist::scene_null; //����go�̂���scene

		std::list<std::shared_ptr<Gameobject>>::iterator go_iterator; //���g�ւ̃C�e���[�^�[(���g����?)

		//�A�^�b�`���ꂽ�R���|�[�l���g�̏���
		void initialize();
		void update();
		void render();
	private:

	public:

		//:::::::::::::::::::::::::
		// pos : �`����W����̑��΍��W
		// r : ���a
		//:::::::::::::::::::::::::
		void set_collider_sphere(vector3 pos, float r);

		//:::::::::::::::::::::::::
		// pos : �`����W����̑��΍��W
		// size : �`���]����� x,y,z �̑���half_size
		// rotate : �`���]����̑��Ή�]
		//:::::::::::::::::::::::::
		void set_collider_box(vector3 pos, vector3 size, quaternion orient);

		//:::::::::::::::::::::::::
		// pos : ���΍��W
		// normal : ���Ζ@��
		//:::::::::::::::::::::::::
		void set_collider_plane(vector3 pos, vector3 normal);


	};

}