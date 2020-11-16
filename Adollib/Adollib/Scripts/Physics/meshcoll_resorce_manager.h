#pragma once
#include <d3d11.h>
#include <unordered_map>

#include "../Math/vector.h"
#include "ALP__meshcoll_data.h"

namespace Adollib
{
	namespace Physics_function {

		class Collider_ResourceManager
		{
		private:
			Collider_ResourceManager() {};
			~Collider_ResourceManager() {};

			static std::unordered_map <std::string, std::vector<Meshcollider_mesh>>	meshes;

		public:
			static std::unordered_map <std::string, std::vector<Vector3>> vertices_;
			static bool CreateMCFromFBX(const char* fbxname, std::vector<Meshcollider_mesh>** mesh); //FBXÇ©ÇÁMeshColliderÇÃê∂ê¨
			static void destroy();

		};
	}
}