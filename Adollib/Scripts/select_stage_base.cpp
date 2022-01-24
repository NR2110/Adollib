
#include "select_stage_base.h"

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Renderer/material_manager.h"
#include "../Adollib/Scripts/Renderer/texture.h"
#include "../Adollib/Scripts/Renderer/renderer_base.h"
#include "../Adollib/Scripts/Physics/collider.h"

using namespace Adollib;

void Select_stage_base::create_selectstage(std::string name, std::string filepath, Vector3 scale) {

	gameobject = Gameobject_manager::createPlane(name);
	gameobject->transform->local_scale = scale;

	std::wstring widestr = std::wstring(filepath.begin(), filepath.end());
	const wchar_t* widecstr = widestr.c_str(); //texture filepass—p‚É•ÏŠ·

	auto material = Material_manager::create_material(name + filepath);
	material->Load_VS("./DefaultShader/default_vs.cso");
	material->Load_PS("./DefaultShader/default_ps_noshadow.cso");
	material->get_texture()->Load(widecstr);

	gameobject->renderer->set_material(material);
}