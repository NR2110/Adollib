#pragma once

#include "object.h"
#include "input.h"
#include "time.h"
#include <string>
#include <memory>

//TODO : ����ȃR���|�[�l���g��肽���Ȃ� �p���g���ĂȂ���
namespace Adollib
{
	class Light;
	class Transfome;

	// ****************************************************
	// gameobject_camera�ɃA�^�b�`����邷�ׂĂɑ΂���x�[�X�N���X // TODO : �C���^�[�t�F�[�X�ɂ��Ă��悢�����H
	// ****************************************************
	class Component_light 
	{
	public:
		Light* gameobject;	// ���̃R���|�[�l���g���A�^�b�`����Ă���GameObject
		Transfome* transform;	// GameObject��Transform�ւ̃|�C���^

		MonoInput* input = nullptr;
		Time* time = nullptr;

	public:
		Component_light(const Component_light&) = delete;
		Component_light& operator=(const Component_light&) = delete;

		Component_light() = default;
		virtual ~Component_light() = default;



		// addComponent���ꂽ�Ƃ��ɌĂ΂��
		virtual void awake() {};

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		virtual void start() {};

		// ���t���[���Ă΂��X�V����
		virtual void update() {};

		// ���t���[���Aupdate()��ɌĂ΂��X�V����
		virtual void lateUpdate() {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��(GO�������ɃX�N���v�g���L���Ȏ����Ă΂��)
		virtual void onEnable() {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		virtual void onDisable() {};

		// removeComponent()�Aclear()���ɌĂ�
		virtual void finalize() {};

	};



}
