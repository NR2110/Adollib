#pragma once

#include "Object.h"
#include "component.h"
#include "../Main/input.h"
#include "time.h"
#include <string>
#include <memory>

namespace Adollib
{
	//�J�����p�̃R���|�[�l���g�N���X �p���s��!
	class Camera_component final : public Component
	{
	public:
		float fov = 60.0f;
		float aspect = 1280 / 720.0f;
		float nearZ = 0.1f;
		float farZ = 1000000.0f;


	private:
		//manager�ɕۑ�����Ă��� ���g�ւ�itr
		std::list<Camera_component*>::iterator this_itr;

	public:

		// addComponent���ꂽ�Ƃ��ɌĂ΂��
		void awake() override;

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		virtual void start() override {};

		// Hierarchy�̕\��(Imgui�̊֐� Imgui::begin,Imgui::end�͂���Ȃ�)
		virtual void Update_hierarchy() override;

		// ���t���[���Ă΂��X�V����
		virtual void update() override {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��(GO�������ɃX�N���v�g���L���Ȏ����Ă΂��)
		virtual void onEnable() override {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		virtual void onDisable() override {};

		// removeComponent()�Aclear()���ɌĂ�
		void finalize() override;

	};



};
