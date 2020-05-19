#pragma once

#include "object.h"

#include <string>
#include <memory>

namespace Adollib
{
	// ****************************************************
	// gameobject�ɃA�^�b�`����邷�ׂĂɑ΂���x�[�X�N���X // TODO : �C���^�[�t�F�[�X�ɂ��Ă��悢�����H
	// ****************************************************
	class Component : public object
	{
	public:

		// �X�V�����̏�����ʂ��邽�߂̃t���O
		bool is_camera_component = false;	// �J�����Ȃ��true //��ԍŌ�ɍX�V����?

	public:
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		Component() = default;
		virtual ~Component() = default;


		// �X�N���v�g���A�^�b�`�����Ƃ��ɌĂ�(transform�̐ݒ�A������O�ɃA�^�b�`�����R���|�[�l���g�܂ł����擾�ł��Ȃ�)
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



		//QUESTION : �Ȃ�������?
		// ==============================================================
		// ����gameobject�ɃA�^�b�`����Ă���R���|�[�l���g���擾����
		// ==============================================================
		// typename T	:	�擾�������R���|�[�l���g��
		// ==============================================================
		// �߂�l T*		:	T������΂��̃|�C���^���A�Ȃ����nullptr��Ԃ�
		// ==============================================================
		//template<typename T>
		//T* get_component() const
		//{
		//	for (auto&& com : gameobject->components)
		//	{
		//		if (typeid(T) == typeid(*com))
		//		{
		//			T* component = dynamic_cast<T*>(com.get());
		//			return component;
		//		}
		//	}
		//	return nullptr;
		//}
	};



}
