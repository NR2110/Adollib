#pragma once

#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"

#include "stage_base.h"
#include "stageparts_tags.h"

namespace Adollib {

	namespace Stage_parts {

		class Stageparts_base : public Component {
		public:
			Stage_base* this_stage = nullptr;      //����parts�����݂��Ă���stage
			Collider* this_coll = nullptr;         //���g��collider

		public:
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

}
