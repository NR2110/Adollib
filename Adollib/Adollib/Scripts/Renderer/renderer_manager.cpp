
#include "renderer_manager.h"

#include "../Object/gameobject.h"

using namespace Adollib;

std::unordered_map<Scenelist, std::list<Renderer*>> Renderer_manager::renderers;

std::list<Renderer*>::iterator Renderer_manager::add_renderer(Renderer* renderer) {
	const auto& Sce = renderer->gameobject->get_scene();
	renderers[Sce].emplace_back(renderer);

	auto itr = renderers[Sce].end();
	--itr;

	return itr;
};

void Renderer_manager::remove_renderer(std::list<Renderer*>::iterator itr) {
	renderers[(*itr)->gameobject->get_scene()].erase(itr);
}


void Renderer_manager::render(Scenelist Sce) {

	for (auto& renderer : renderers[Sce]) {
		renderer->render();
	}

}