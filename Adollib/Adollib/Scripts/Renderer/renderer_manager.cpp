
#include "renderer_manager.h"

#include "renderer_base.h"

#include "Shader/constant_buffer.h"
#include "../Main/systems.h"
#include "../Object/gameobject.h"
#include "../Object/component_camera.h"
#include "../Object/component_light.h"
#include "../Physics/ALP__physics_manager.h"

#include "../Imgui/work_meter.h"
#include "../Imgui/imgui_manager.h"

using namespace Adollib;
using namespace ConstantBuffer;

std::unordered_map<Scenelist, std::list<Renderer_base*>> Renderer_manager::renderers;

ComPtr<ID3D11Buffer> Renderer_manager::light_cb;

void Renderer_manager::awake() {

	Systems::CreateConstantBuffer(&light_cb, sizeof(ConstantBufferPerLight));
}

std::list<Renderer_base*>::iterator Renderer_manager::add_renderer(Renderer_base* renderer) {
	const auto& Sce = renderer->gameobject->get_scene();
	renderers[Sce].emplace_back(renderer);

	auto itr = renderers[Sce].end();
	--itr;

	return itr;
};

void Renderer_manager::remove_renderer(std::list<Renderer_base*>::iterator itr) {
	renderers[(*itr)->gameobject->get_scene()].erase(itr);
}

void Renderer_manager::render(const std::list<Camera_component*>& cameras,const std::list<Light_component*>& lights, Scenelist Sce) {

	Systems::Clear();

	// �O�p�`�̕`����@
	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Work_meter::tag_start("render");
	if (Sce == Scenelist::scene_null)return;

	// light�����R���X�^���g�o�b�t�@�[�ɃZ�b�g����
	set_light_Constantbuffer(lights);

	//���̃V�[���̃J�����̐�������
	for (const auto& camera : cameras) {
		Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (camera->gameobject->active == false)continue;

		// camera�̎���texture��clear
		camera->clear();

		// camera�����R���X�^���g�o�b�t�@�[�ɃZ�b�g����
		camera->set_Constantbuffer();

		// �`�悷��scene
		auto render_scene = camera->render_scene;

		Work_meter::start("render_obj");
		// render���Ă�
		for (auto& renderer : renderers[render_scene]) {
			renderer->render();
		}
		Work_meter::stop("render_obj");

		// posteffect�̏��� & main��RTV�ɕ`��
		camera->posteffect_render();

		// collider��render
		Physics_function::Physics_manager::render_collider(Sce);


		Work_meter::tag_stop();

	}

	Systems::SetRenderTargetView();
	Adollib::Imgui_manager::render();

}

void Renderer_manager::set_light_Constantbuffer(const std::list<Light_component*>& lights) {
	//CB : ConstantBufferPerLight
	ConstantBufferPerLight l_cb;
	l_cb.AmbientColor = Ambient.get_XM4();
	l_cb.LightDir = LightDir.get_XM4();
	l_cb.LightColor = DirLightColor.get_XM4();

	//�R���X�^���g�o�b�t�@�ɓn�����߂�pointlight,spotlight�̔z��𐮗�
	POINTLIGHT PointLight[POINTMAX] = { 0 };
	SPOTLIGHT SpotLight[SPOTMAX] = { 0 };
	{
		int point_num = 0;
		int spot_num = 0;
		for (const auto& light : lights) {
			for (u_int o = 0; o < light->PointLight.size(); o++) {
				if (light->gameobject->active == false)return;
				PointLight[point_num] = *light->PointLight[o];
				//	PointLight[point_num].pos = (*itr_li->get()->PointLight[o]->pos )+( *itr_li->get()->transform->position);
				point_num++;
			}

			for (u_int o = 0; o < light->SpotLight.size(); o++) {
				if (light->gameobject->active == false)return;
				SpotLight[spot_num] = *light->SpotLight[o];
				spot_num++;
			}
		}
	}

	memcpy(l_cb.PointLight, PointLight, sizeof(POINTLIGHT) * POINTMAX);
	memcpy(l_cb.SpotLight, SpotLight, sizeof(SPOTLIGHT) * SPOTMAX);
	Systems::DeviceContext->UpdateSubresource(light_cb.Get(), 0, NULL, &l_cb, 0, 0);
	Systems::DeviceContext->VSSetConstantBuffers(4, 1, light_cb.GetAddressOf());
	Systems::DeviceContext->PSSetConstantBuffers(4, 1, light_cb.GetAddressOf());
}