#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {

		// �w���tag��1�ł���Γ����M�~�b�N
		class Area_respown_set : public Stageparts_base {
		public:
			int respown_num = -1;
			Vector3 respown_position = Vector3(0);

		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override;

			// ���t���[���Ă΂��X�V����
			void update() override;

		};


	}
}

