#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>
#include <DirectXMath.h>

namespace Adollib {

	class Debug {
	private:
		struct base {
			std::string param_name;
			DirectX::XMFLOAT4 param;
			unsigned int param_num;
		};
		static std::unordered_map<std::string, std::vector<base>> debug_params;
		static std::vector<std::string>tags;

		static std::vector<base> notag_debug_param;

		static bool stop_flag;

	public:
		static void set(std::string name, DirectX::XMFLOAT4 xyzw);
		static void set(std::string name, DirectX::XMFLOAT3 xyz);
		static void set(std::string name, float x = 0, float y = 0, float z = 0, float w = 0);

		static void set(std::string tabname, std::string name, float x, float y, float z, float w);
		static void set(std::string tabname, std::string name, DirectX::XMFLOAT4 xyzw);
		static void set(std::string tabname, std::string name, DirectX::XMFLOAT3 xyz) ;
																					  
		static void render();

	};
}