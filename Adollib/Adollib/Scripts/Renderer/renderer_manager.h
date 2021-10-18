#pragma once

#include <vector>
#include <string>
#include <list>
#include <unordered_map>

#include "Mesh/mesh.h"
#include "Mesh/material.h"
#include "../Scene/scene_list.h"

#include "renderer.h"

namespace Adollib {

	class Renderer_manager {

	private:
		static std::unordered_map<Scenelist, std::list<Renderer*>> renderers;


	public:
		static std::list<Renderer*>::iterator add_renderer(Renderer* renderer);

		static void remove_renderer(std::list<Renderer*>::iterator itr);

	public:
		static void render(Scenelist Sce);

	};


}