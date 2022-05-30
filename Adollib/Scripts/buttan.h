#pragma once

// �{�^�� ������stage_base�̕ϐ��̎w���bit������

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

#include "../../Scripts/Main/mono_audio.h"

namespace Adollib{

	namespace Stage_parts {

		class Buttan : public Stageparts_base {
		public:
			Stageparts_tagbit tag = Stageparts_tags::None; //���g�̊Ǘ�����bit

			float base_scale_y = 1;
			Vector3 base_material_color = Vector3(0.5f);

			// trigger�Ƃ��Ďg�p���邩
			int is_use_trigger = 0;
		private:
			float timer = 0; //����Ă������̊Ԃ�ON�ɂȂ�悤�ɂ��邽��(�����������������Ȃ�)
			float trigger_timer = 0;

			bool is_trigger_pushed = false;

			float se_on_timer = 0;
			float se_off_timer = 0;

		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {
				constexpr float color_cheng_time = 5.5f;

				float pearent_y_inve_scale = 1;
				float pearent_y_scale = 1;
				if (this_coll->gameobject->parent() != nullptr)pearent_y_scale = this_coll->gameobject->parent()->transform->scale.y;
				pearent_y_inve_scale = 1 / pearent_y_scale;

				// ��������ςȂ�
				if (is_use_trigger == 2) {
					if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) timer = 0.25f;
				}
				// trigger
				else if (is_use_trigger == 1) {

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
				// state
				else {
					if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) timer = 0.25f;
					else timer -= Al_Global::second_per_frame;
				}

				// �G��Ă���Ƃ�button��������
				if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) {
					// scale�̒���
					transform->local_scale.y = ALClamp(transform->local_scale.y * pearent_y_scale - 0.2f * Al_Global::second_per_frame, base_scale_y, base_scale_y + 0.2f) * pearent_y_inve_scale;
				}
				else {
					// scale�̒���
					transform->local_scale.y = ALClamp(transform->local_scale.y * pearent_y_scale + 0.2f * Al_Global::second_per_frame, base_scale_y, base_scale_y + 0.2f) * pearent_y_inve_scale;
				}

				// player���������̂ɐG��Ă�����
				if (timer > 0) {

					// bit�𗧂Ă�
					this_stage->tags |= tag;

					// �F�̕ύX
					gameobject->renderer->color.x = ALClamp(gameobject->renderer->color.x + (base_material_color.y - base_material_color.x) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
					gameobject->renderer->color.y = ALClamp(gameobject->renderer->color.y + (base_material_color.x - base_material_color.y) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);

					if (se_on_timer < 0)MonoAudio::PlayMusic(Music::SE_Button_on, false);
					se_on_timer = 0.2f;
				}
				else {
					// bit�𗧂Ă�
					this_stage->tags &= ~tag;

					// �F�̕ύX
					gameobject->renderer->color.x = ALClamp(gameobject->renderer->color.x - (base_material_color.y - base_material_color.x) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
					gameobject->renderer->color.y = ALClamp(gameobject->renderer->color.y - (base_material_color.x - base_material_color.y) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);

					if (se_off_timer < 0)MonoAudio::PlayMusic(Music::SE_Button_off, false);
					se_off_timer = 0.2f;
				}


				se_on_timer -= Al_Global::second_per_frame;
				se_off_timer -= Al_Global::second_per_frame;
			};

		};


	}
}

