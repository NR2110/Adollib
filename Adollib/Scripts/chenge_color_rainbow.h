#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Renderer/renderer_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// �w���tag��1�ł���Γ����M�~�b�N
		class Change_color_reinbow : public Stageparts_base {
		public:
			Stageparts_tagbit tag = Stageparts_tags::None; //���g�̊Ǘ�����bit

			float timer = 0;

			Vector3 base_color = Vector3(1, 1, 1);

		private:


		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {
				timer += time->deltaTime();

				// tag�������Ă�����timer��i�߂�
				//if (this_stage->tags & tag)	gameobject->renderer->color = Vector4(Al_Global::get_gaming(timer, 2, 196 / 255.0f, 1), 1);
				//else  gameobject->renderer->color = Vector4(base_color, 1);


				Vector4 color = Vector4(base_color, 1);
				if (this_stage->tags & tag) color = Vector4(Al_Global::get_gaming(timer, 2, 196 / 255.0f, 1), 1);

				if (gameobject->renderer != nullptr)gameobject->renderer->color = color;
				for (auto& child : *gameobject->children())if (child->renderer != nullptr)child->renderer->color = color;

			};

		};


	}
}

