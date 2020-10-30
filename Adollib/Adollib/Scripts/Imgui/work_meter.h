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

		static std::unordered_map<std::string, std::unordered_map<std::string, meter>> start_stop;
		static std::unordered_map<std::string, std::unordered_map<std::string, float[max_ * 2]>> meters;

		static std::unordered_map<std::string,std::vector<std::string>> names;
		static std::unordered_map < std::string, std::vector<bool>> name_flags;
		static std::vector<std::string> tags;

		static std::string now_tag;
	public:

		static bool render();
		static bool start(std::string);
		static bool stop(std::string);

		static bool tag_start(std::string); //tag
		static bool tag_stop (); //tag


	};
}