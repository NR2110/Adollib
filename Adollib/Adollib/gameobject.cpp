#include "gameobject.h"

#include "systems.h"
namespace Adollib{

	void Gameobject::initialize() {

		std::list <std::shared_ptr<Component>>::iterator itr = components.begin();
		itr++;
		std::list <std::shared_ptr<Component>>::iterator itr_end = components.end();

		Systems::CreateConstantBuffer(&world_cb, sizeof(ConstantBufferPerGO));

		for (; itr != itr_end; itr++) {
			itr->get()->start();
		}

	}

	void Gameobject::update() {

		std::list <std::shared_ptr<Component>>::iterator itr = components.begin();
		itr++;
		std::list <std::shared_ptr<Component>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->update();
		}



	}

	void Gameobject::render() {

		if (Systems::BS_type != material->BS_state) Systems::SetBlendState(material->BS_state);
		if (Systems::RS_type != material->RS_state) Systems::SetRasterizerState(material->RS_state);
		if (Systems::DS_type != material->DS_state) Systems::SetDephtStencilState(material->DS_state);

		ConstantBufferPerGO g_cb;
		g_cb.world = get_word_matrix().get_XMMATRIX();
		Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());


	}




}