#pragma once
#include "stage_base.h"

namespace Adollib {

	class Stage_demo : public Stage_base {


	private:

		Gameobject* set_tree(const Vector3& pos, const Vector3& scale, const Vector3& rotate);
		Gameobject* set_buttan(const Vector3& pos, const Vector3& scale, const Vector3& rotate, Stage_parts::Stageparts_tagbit tag, const Vector3& color = Vector3(1, 1, 1));
		Gameobject* set_door  (
			const Vector3& start_pos, const Vector3& goal_pos,
			const Vector3& start_rot, const Vector3& goal_rot,
			float pos_speed,float rot_speed,
			const Vector3& scale,
			Stage_parts::Stageparts_tagbit tag,
			const Vector3& color = Vector3(1, 1, 1)
		);
		Gameobject* set_rope(const Vector3& pos, const Vector3& scale, const Vector3& rotate, const Vector3& color = Vector3(1, 1, 1));

	public:
		void awake() override;

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start()override;

		// ���t���[���Ă΂��X�V����
		void update()override;

		void Update_hierarchy() override {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable()override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable()override;
	};
}