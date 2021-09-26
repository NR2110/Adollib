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
			long long duration_time;
		};

		static const int max_ = 120;

		static std::unordered_map<std::string, std::unordered_map<std::string, meter>> start_stop; //<tag名前,<meter名前,meter>> メーター名からstopを呼べるように
		static std::unordered_map<std::string, std::unordered_map<std::string, float[max_ * 2]>> meters;

		static std::unordered_map<std::string,std::vector<std::string>> names; //mapを回すために保存
		static std::unordered_map < std::string, std::vector<bool>> name_flags;
		static std::vector<std::string> tags;

		static std::string now_tag;
	public:

		static bool render();
		static bool start(const std::string&);
		static bool stop(const std::string&);
		static void set(const std::string& name, const float& value);

		static bool tag_start(const std::string&); //tag
		static bool tag_stop (); //tag


	};
}