#pragma once

#include <list>
#include <memory>

#include "gameobject.h"
#include "component.h"
#include "transform.h"
#include "scene_list.h"

namespace Adollib {

	class Camera : public object {
	public:
		std::string name = std::string("null"); //����go�̖��O(�����p)

		Transfome* transform = nullptr; //�`��p�̏��

		std::list <std::shared_ptr<Component>> components; //�A�^�b�`����Ă���Conponent�̃|�C���^

		Camera* pearent = nullptr; //�e�ւ̃|�C���^
		std::list<std::shared_ptr<Gameobject>> children; //�ւ̃|�C���^

		bool active = true; //false�Ȃ�X�V�A�`����~�߂�

		Scenelist this_scene = Scenelist::scene_null; //����go�̂���scene

		std::list<std::shared_ptr<Camera>>::iterator go_iterator; //���g�ւ̃C�e���[�^�[(���g����?)

		//�A�^�b�`���ꂽ�R���|�[�l���g�̏���
		void initialize();
		void update();
		void render() {};
	private:

	public:

	};

}