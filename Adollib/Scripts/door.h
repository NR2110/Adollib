#pragma once

#pragma once

#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"

#include "stage_base.h"
#include "stageparts_tags.h"

namespace Adollib {

	namespace Stage_parts {
		// �w���tag��1�ł���Γ����M�~�b�N
		class Door : public Component {
			Stage_base* tihs_stage; //����parts�����݂��Ă���stage
			Collider* this_coll; //���g��collider

			Stageparts_tagbit flag = Stageparts_tags::None; //���g�̊Ǘ�����bit

			Vector3 dir = Vector3(0, 1, 0); //��������
			float move_dis = 10; //��������
		private:

		public:
			// ���t���[���Ă΂��X�V����
			void update() override{

				//�w���bit�������Ă�����͂�������
				if (tihs_stage->tags & flag) {





				}
			};

		};


	}
}

