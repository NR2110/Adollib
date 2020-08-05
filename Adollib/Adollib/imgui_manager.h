#pragma once
#include <windows.h>


namespace Adollib {

	namespace Imgui_manager {

		bool init(HWND hwnd);
		bool update(MSG hmsg);
		bool render();
		bool destroy();
	}

}