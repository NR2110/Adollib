#pragma once

#include "object.h"
#include "../Main/input.h"
#include "time.h"
#include <string>
#include <memory>

namespace Adollib
{
	class Gameobject;
	class Transfome;

	// ****************************************************
	// gameobject�ɃA�^�b�`����邷�ׂĂɑ΂���x�[�X�N���X // TODO : �C���^�[�t�F�[�X�ɂ��Ă��悢�����H
	// ****************************************************
	class Component 
	{
	public:
		std::list <std::shared_ptr<Component>>::iterator com_itr; //���g�ւ̃C�e���[�^

		Gameobject* gameobject;	// ���̃R���|�[�l���g���A�^�b�`����Ă���GameObject
		Transfome*  transform;	// GameObject��Transform�ւ̃|�C���^

		MonoInput* input = nullptr;
		Time* time = nullptr;

	public:
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		Component() = default;
		virtual ~Component() = default;


		// addComponent���ꂽ�Ƃ��ɌĂ΂��
		virtual void awake() {};

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		virtual void start() {};

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
