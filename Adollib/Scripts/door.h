#pragma once

#include "stageparts_base.h"

namespace Adollib {

	namespace Stage_parts{
		// �w���tag��1�ł���Γ����M�~�b�N
		class Door : public Stageparts_base {

			Stageparts_tagbit flag = Stageparts_tags::None; //���g�̊Ǘ�����bit

			Vector3 dir = Vector3(0, 1, 0); //��������
			float move_dis = 10; //��������

		private:

		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override{

				//�w���bit�������Ă�����͂�������
				if (this_stage->tags & flag) {





				}
			};

		};


	}
}

