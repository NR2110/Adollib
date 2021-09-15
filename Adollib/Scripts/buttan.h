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

		private:
			float timer = 0; //����Ă������̊Ԃ�ON�ɂȂ�悤�ɂ��邽��(�����������������Ȃ�)
		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {
				constexpr float color_cheng_time = 5.5f;

				if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) timer = 0.25f;
				else timer -= Al_Global::second_per_frame;

				//player���������̂ɐG��Ă�����
				if (timer > 0) {

					//bit�𗧂Ă�
					this_stage->tags |= tag;

					//�F�̕ύX
					gameobject->material->color.x = ALClamp(gameobject->material->color.x + (base_material_color.y - base_material_color.x) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
					gameobject->material->color.y = ALClamp(gameobject->material->color.y + (base_material_color.x - base_material_color.y) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);

					//scale�̒���
					transform->local_scale.y = ALClamp(transform->local_scale.y - 0.2f * Al_Global::second_per_frame, base_scale_y, base_scale_y + 0.2f);

				}
				else {
					//bit�𗧂Ă�
					this_stage->tags &= ~tag;

					//�F�̕ύX
					gameobject->material->color.x = ALClamp(gameobject->material->color.x - (base_material_color.y - base_material_color.x) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);
					gameobject->material->color.y = ALClamp(gameobject->material->color.y - (base_material_color.x - base_material_color.y) * color_cheng_time * Al_Global::second_per_frame, base_material_color.y, base_material_color.x);

					//scale�̒���
					transform->local_scale.y = ALClamp(transform->local_scale.y + 0.2f * Al_Global::second_per_frame, base_scale_y, base_scale_y + 0.2f);
				}

			};

		};


	}
}

