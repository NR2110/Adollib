#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>

namespace Adollib {
	class Work_meter {
	private:
		struct meter {
			LARGE_INTEGER start;
			LARGE_INTEGER stop;
		};

		static const int max_ = 120;
		static float max_num;
		static int start_num;
		static float before;
		static std::unordered_map<std::string, meter> s_g;
		static std::unordered_map<std::string, float[max_ * 2]> meters;
		static std::vector<std::string> names;
	public:

		static bool render();
		static bool start(std::string);
		static bool stop(std::string);



	};
}