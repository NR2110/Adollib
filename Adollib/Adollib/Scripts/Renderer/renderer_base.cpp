
#include "renderer_base.h"

#include "../Object/gameobject.h"
#include "renderer_manager.h"
#include "material_manager.h"

#include "../Main/systems.h"
#include "Shader/constant_buffer.h"
#include "Shader/vertex_format.h"

#include "material.h"
#include "texture.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Renderer_base::shader_activete(const bool& is_PSshader_update) {
	material->shader_activate(is_PSshader_update);
}


void Renderer_base::awake() {
	// すでにrendererがセットされていた場合 外す
	std::vector<Mesh::mesh>* oldmesh = nullptr;
	if (gameobject->renderer != nullptr) {
		oldmesh = gameobject->renderer->get_mesh();
		gameobject->removeComponent(gameobject->renderer);

		gameobject->renderer = nullptr;
	}


	this_itr = Renderer_manager::add_renderer(this);

	Systems::CreateConstantBuffer(world_cb.ReleaseAndGetAddressOf(), sizeof(ConstantBufferPerGO));
	Systems::CreateConstantBuffer(Mat_cb.ReleaseAndGetAddressOf(), sizeof(ConstantBufferPerMaterial));

	//material = std::make_shared<Material>();
	material = Material_manager::find_material("default_material");

	init();

	set_meshes(oldmesh);
}

void Renderer_base::finalize() {
	Renderer_manager::remove_renderer(this_itr);
}

void Renderer_base::set_instance(Instance& instance) {

	auto world = matrix_world(transform->scale, transform->orientation.get_rotate_matrix(), transform->position);
	instance.transformMatrix = world;
	instance.texcoordTransform = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	instance.color = color;
}