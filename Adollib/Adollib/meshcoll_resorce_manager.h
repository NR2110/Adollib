#pragma once
#include <d3d11.h>
#include <unordered_map>

#include "vector.h"

namespace Adollib
{
	class Collider_ResourceManager
	{
	public:
		static HRESULT CreateMCFromFBX(const char* fbxname, std::vector<int>** index, std::vector<vector3>** vertex); //FBX����MeshCollider�̐���
		static void destroy();

	private:
		Collider_ResourceManager() {};
		~Collider_ResourceManager() {};

		static std::unordered_map <std::string, std::vector<int>>	indexes;
		static std::unordered_map <std::string, std::vector<vector3>> vertexes;
	};
}