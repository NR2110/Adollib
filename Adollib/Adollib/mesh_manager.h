#pragma once

#include "systems.h"
#include "mesh.h"

namespace Adollib {
	class Mesh_manager {
	private:
		static std::map<std::string ::unique_ptr<Mesh>> meshes;

	public:

		static void create(const std::string fbx_filename, const std::string filePath = "");



	};


}