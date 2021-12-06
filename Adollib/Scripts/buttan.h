#pragma once

// �{�^�� ������stage_base�̕ϐ��̎w���bit������

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib{

	namespace Stage_parts {

		class Buttan : public Stageparts_base {
		public:
			Stageparts_tagbit tag = Stageparts_tags::None; //���g�̊Ǘ�����bit

			float base_scale_y = 1;
			Vector3 base_material_color = Vector3(0.5f);

			// trigger�Ƃ��Ďg�p���邩
			bool is_use_trigger = false;
		private:
			float timer = 0; //����Ă������̊Ԃ�ON�ɂȂ�悤�ɂ��邽��(�����������������Ȃ�)
			float trigger_timer = 0;

			bool is_trigger_pushed = false;

		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {
				constexpr float color_cheng_time = 5.5f;

				if (is_use_trigger) {

					// �O�̃t���[��������Ă��Ȃ��Ƃ�
					if (trigger_timer < 0){
						// �����ꂽ��
						if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) is_trigger_pushed = !is_trigger_pushed; //�؂�ւ���
					}

					if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage))trigger_timer = 1;
					else trigger_timer -= Al_Global::second_per_frame;

					//
					if (is_trigger_pushed)  timer = 1;
					else timer = -1;

				}
				else {
					if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) timer = 0.25f;
					else timer -= Al_Global::second_per_frame;
				}

				// �G��Ă���Ƃ�button��������
				if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) {
					// scale�̒���
					transform->local_scale.y = ALClamp(transform->local_scale.y - 0.2f * Al_Global::second_per_frame, base_scale_y, base_scale_y + 0.2f);
				}
				else {
					// scale�̒���
					transform->local_scale.y = ALClamp(transform->local_scale.y + 0.2f * Al_Global::second_per_frame, base_scale_y, base_scale_y + 0.2f);
				}

				// player���������̂ɐG��Ă�����
				if (timer > 0) {

					// bit�𗧂Ă�
					this_stage->tags |= tag;

					// �F�̕ύX
					gameobject->material->color.x = ALClamp(gameobject->material->color.x + (base_material_color.y - base_material_color.x) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
					gameobject->material->color.y = ALClamp(gameobject->material->color.y + (base_material_color.x - base_material_color.y) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);

				}
				else {
					// bit�𗧂Ă�
					this_stage->tags &= ~tag;

					// �F�̕ύX
					gameobject->material->color.x = ALClamp(gameobject->material->color.x - (base_material_color.y - base_material_color.x) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
					gameobject->material->color.y = ALClamp(gameobject->material->color.y - (base_material_color.x - base_material_color.y) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
				}

			};

		};


	}
}

