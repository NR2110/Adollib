#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// �w���tag��1�ł���Γ����M�~�b�N
		class Event_exclamation : public Stageparts_base {
		public:

			Vector3 start_pos;

		private:

			float sumt = 0;

		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {

				sumt += time->deltaTime();

				transform->local_pos = start_pos + cosf(sumt * 2) * Vector3(0, 0.3f, 0);
				transform->local_orient = quaternion_from_euler(0, sumt * 10, 0);


			};

		};


	}
}

