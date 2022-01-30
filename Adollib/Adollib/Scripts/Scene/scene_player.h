#pragma once

#include "scene.h"

namespace Adollib {

	class Scene_player : public Scene {

	public:
		int start_stage_num = 0;

	public:
		void awake() override {};

		void initialize() override;

	private:

	};
}