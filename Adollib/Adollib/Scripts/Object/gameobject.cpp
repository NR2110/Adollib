#include "gameobject.h"
#include "../Main/systems.h"

#include "../Mesh/material_for_collider.h"
#include "../Main/Adollib.h"
#include "../Mesh/frustum_culling.h"

#include "../Shader/constant_buffer.h"

using namespace Adollib;
using namespace ConstantBuffer;

void Gameobject::initialize() {

	std::list <std::shared_ptr<Component>>::iterator itr = components.begin();
	std::list <std::shared_ptr<Component>>::iterator itr_end = components.end();

	Systems::CreateConstantBuffer(&world_cb, sizeof(ConstantBufferPerGO));

	for (; itr != itr_end; itr++) {
		itr->get()->start();
	}

}

void Gameobject::update() {

	std::list <std::shared_ptr<Component>>::iterator itr = components.begin();
	std::list <std::shared_ptr<Component>>::iterator itr_end = components.end();

	for (; itr != itr_end; itr++) {
		itr->get()->update();
	}



}

void Gameobject::render() {
	if (no_material == false) {

		//CB : ConstantBufferPerOBJ
		ConstantBufferPerGO g_cb;
		g_cb.world = matrix_world(transform->scale, transform->orientation.get_rotate_matrix(), transform->position).get_XMFLOAT4X4();
		Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

		//Ž‹‘äƒJƒŠƒ“ƒO—p
		FrustumCulling::update_obj(this);
		FrustumCulling::frustum_culling_init();

		material->render();
	}

}

