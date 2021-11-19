#pragma once

#include "Object.h"
#include "../Main/input.h"
#include "time.h"
#include <string>
#include <memory>

namespace Adollib
{
	class Gameobject;
	class Transform;

	// gameobject�ɃA�^�b�`�����x�[�X�N���X
	class Component
	{
	public:
		std::list<Component*>::iterator com_itr; //���g�ւ̃C�e���[�^

		Gameobject* gameobject = nullptr;	// ���̃R���|�[�l���g���A�^�b�`����Ă���GameObject
		Transform*  transform = nullptr;	// GameObject��Transform�ւ̃|�C���^

		MonoInput* input = nullptr;
		std::shared_ptr<Time> time = nullptr;

		std::string name; //Hierarchy�ɕ\�����閼�O

	public:
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		Component() = default;
		virtual ~Component() = default;


		// addComponent���ꂽ�Ƃ��ɌĂ΂��
		virtual void awake() {};

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		virtual void start() {};

		// Hierarchy�̕\��(Imgui�̊֐� Imgui::begin,Imgui::end�͂���Ȃ�)
		virtual void Update_hierarchy() {};

		// ���t���[���Ă΂��X�V����
		virtual void update() {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��(GO�������ɃX�N���v�g���L���Ȏ����Ă΂��)
		virtual void onEnable() {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		virtual void onDisable() {};

		// removeComponent()�Aclear()���ɌĂ�
		virtual void finalize() {};

	};



}
