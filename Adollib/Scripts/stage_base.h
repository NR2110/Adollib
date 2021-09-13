#pragma once

#include "../Adollib/Scripts/Object/component.h"


#include "stageparts_tags.h"

namespace Adollib {

	class Collider;

	class Stage_base : public Component {
	public:
		Stage_parts::Stageparts_tagbit tags; //����stage������tag

		float y_respown_value = 0; // Y������ȉ�����respown�����

	private:


	protected:
		Collider* set_sphere (Vector3 pos = Vector3(0), float r = 1, Vector3 color = Vector3(1, 1, 1), bool is_static = true);
		Collider* set_capsule(Vector3 pos = Vector3(0), float r = 1, float length = 1, Vector3 rotate = Vector3(0), Vector3 color = Vector3(1, 1, 1), bool is_static = true);
		Collider* set_box    (Vector3 pos = Vector3(0), Vector3 size = Vector3(1), Vector3 rotate = Vector3(0), Vector3 color = Vector3(1, 1, 1), bool is_static = true);
		Collider* set_meshbox(Vector3 pos = Vector3(0), Vector3 size = Vector3(1), Vector3 rotate = Vector3(0), Vector3 color = Vector3(1, 1, 1), bool is_static = true);
		Collider* set_plane  (Vector3 pos = Vector3(0), Vector3 normal = Vector3(0,1,0), Vector3 color = Vector3(1, 1, 1), bool is_static = true);

	public:
		void tag_reset() { tags = 0; };


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