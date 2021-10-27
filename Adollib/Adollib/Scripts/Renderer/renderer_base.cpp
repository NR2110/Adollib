
#include "renderer_base.h"

#include "../Object/gameobject.h"
#include "renderer_manager.h"

#include "../Main/systems.h"
#include "Shader/constant_buffer.h"
#include "Shader/vertex_format.h"

#include "material.h"
#include "texture.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Renderer_base::awake() {
	this_itr = Renderer_manager::add_renderer(this);

	Systems::CreateConstantBuffer(&world_cb, sizeof(ConstantBufferPerGO));
	Systems::CreateConstantBuffer(&Mat_cb, sizeof(ConstantBufferPerMaterial));

	material = std::make_shared<Material>();
	texture = std::make_shared<Texture>();

	material->Load_VS("./DefaultShader/default_vs.cso");
	material->Load_PS("./DefaultShader/default_ps.cso");

	texture->Load(L"./DefaultModel/white.png");

	init();
}

void Renderer_base::finalize() {
	Renderer_manager::remove_renderer(this_itr);

	//delete material;
}