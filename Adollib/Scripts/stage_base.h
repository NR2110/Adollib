#pragma once

#include "../Adollib/Scripts/Object/component.h"

#include <vector>
#include "stageparts_tags.h"
#include "stage_types.h"

namespace Adollib {

	class Gameobject;
	class Collider;

	class Stage_base : public Component {
	public:
		Stage_parts::Stageparts_tagbit tags; //����stage������tag �X�C�b�`�Ȃǂ�flag�̊Ǘ�

		int respown_num = -1; // �O��respown�ʒu��respown���Ȃ��悤�� respown����int�̒l����������

		float y_respown_limit = -150; // Y������ȉ�����respown�����
		float y_respown_pos = 100; // respown�����Ƃ���Y�̍��W

		float y_player_respown_limit = -130; // Y������ȉ�����respown����� player��stage�̏��������respown����Ăق�������ϐ�������
		Vector3 player_respown_pos;
		int player_respown_num = 0; // ���݂�respownpos�̔ԍ� �O��respownpos�ōX�V���Ȃ��悤�ɂ��邽��


	protected:
		std::vector<Gameobject*> stage_parts; // ����stage���Ǘ����Ă���GO�̃|�C���^

	private:


		void tag_reset() { tags = 0; };

	protected:
		Collider* set_sphere (const Vector3& pos = Vector3(0), const float& r = 1, const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);
		Collider* set_capsule(const Vector3& pos = Vector3(0), const float& r = 1, const float& length = 1, const Vector3& rotate = Vector3(0), const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);
		Collider* set_box    (const Vector3& pos = Vector3(0), const Vector3& size = Vector3(1), const Vector3& rotate = Vector3(0), const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr,bool is_static = true);
		Collider* set_meshbox(const Vector3& pos = Vector3(0), const Vector3& size = Vector3(1), const Vector3& rotate = Vector3(0), const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);
		Gameobject* set_plane  (const Vector3& pos = Vector3(0), const Vector3& size = Vector3(1), const Vector3& rotate = Vector3(0), const Vector3& color = Vector3(1, 1, 1), Gameobject* pearent = nullptr, bool is_static = true);

		Gameobject* set_respown_area(const Vector3& pos, const Vector3& size, const Vector3& rotate, float respown_num, Vector3 respown_pos, Gameobject* pearent = nullptr);
		Gameobject* set_goal_area(const Vector3& pos, const Vector3& size, const Vector3& rotate, Stage_types next_stage, Gameobject* pearent = nullptr);

	public:
		// stage_manager���Ă�awake ����stage�ɐ؂�ւ�����Ƃ��ɌĂ΂��
		virtual void stage_awake() = 0;

		// stage_manager���Ă�destroy ����stage���玟�ɐ؂�ւ�����Ƃ��ɌĂ΂��
		virtual void stage_destroy() = 0;

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