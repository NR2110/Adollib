#pragma once

#include "scene.h"

namespace Adollib {

	class Scene_game : public Scene {

	public:
		int start_stage_num = 0;

	public:

		void initialize() override;

		void destroy() override;

	private:

	};
}