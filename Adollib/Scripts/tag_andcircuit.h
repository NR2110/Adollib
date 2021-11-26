#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// �w���tag��1�ł���Γ����M�~�b�N
		class Tag_Andcicuit : public Stageparts_base {
		public:

			Stageparts_tagbit in_tag0 = Stageparts_tags::None; //in_tag0,in_tag1�������Ă���Ƃ�out_tag�𗧂Ă�
			Stageparts_tagbit in_tag1 = Stageparts_tags::None; //in_tag0,in_tag1�������Ă���Ƃ�out_tag�𗧂Ă�
			Stageparts_tagbit out_tag = Stageparts_tags::None; //in_tag0,in_tag1�������Ă���Ƃ�out_tag�𗧂Ă�

		private:



		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {

				if (this_stage->tags & in_tag0 && this_stage->tags & in_tag1) this_stage->tags |= out_tag;
				else this_stage->tags &= ~out_tag;

			};

		};


	}
}

