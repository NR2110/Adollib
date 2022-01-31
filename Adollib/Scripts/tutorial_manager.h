#pragma once

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component.h"

namespace Adollib {

	class Material;

	class Tutrial_manager : public Component{

	private:
		std::shared_ptr<Material> mat_tutrial_move = nullptr;
		std::shared_ptr<Material> mat_tutrial_camera = nullptr;
		std::shared_ptr<Material> mat_tutrial_hand = nullptr;


	public:



	public:
		void awake();

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start();

		// ���t���[���Ă΂��X�V����
		void update();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable() {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable() {};


	};


}