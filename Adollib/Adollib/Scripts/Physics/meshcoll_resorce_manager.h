#pragma once
#include <d3d11.h>
#include <unordered_map>

#include "../Math/vector.h"

namespace Adollib
{
	class Collider_ResourceManager
	{
	public:
		static HRESULT CreateMCFromFBX(const char* fbxname, std::vector<int>** index, std::vector<Vector3>** vertex); //FBXÇ©ÇÁMeshColliderÇÃê∂ê¨
		static void destroy();

	private:
		Collider_ResourceManager() {};
		~Collider_ResourceManager() {};

		static std::unordered_map <std::string, std::vector<int>>	indexes;
		static std::unordered_map <std::string, std::vector<Vector3>> vertexes;
	};
}