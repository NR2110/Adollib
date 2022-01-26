
#include "renderer_manager.h"

#include "renderer_base.h"
#include "directional_shadow.h"
#include "sky_sphere.h"
#include "material_manager.h"

#include "Shader/constant_buffer.h"
#include "../Main/systems.h"
#include "../Object/gameobject.h"
#include "../Object/component_camera.h"
#include "../Object/component_light.h"
#include "../Physics/ALP__physics_manager.h"

#include "../Imgui/work_meter.h"
#include "../Imgui/imgui_manager.h"
#include "../Imgui/debug.h"

#include <algorithm>

//#define use_instancing_batch

using namespace Adollib;
using namespace ConstantBuffer;

std::unordered_map<Scenelist, std::list<Renderer_base*>> Renderer_manager::renderers;
Microsoft::WRL::ComPtr<ID3D11Buffer> Renderer_manager::instanceBuffer;
std::vector<Renderer_base*> Renderer_manager::sorted_renderers;
std::vector<Render_count> Renderer_manager::render_counts; //instancing�`��̂��߂̔z�� ������for�ŉ񂵂ĕ`����s��

ComPtr<ID3D11Buffer> Renderer_manager::light_cb;

void Renderer_manager::awake() {

	Systems::CreateConstantBuffer(light_cb.ReleaseAndGetAddressOf(), sizeof(ConstantBufferPerLight));

	constexpr int maxElements = 10000;
	HRESULT hr = S_OK;
	std::shared_ptr<Instance> instances(new Instance[maxElements]);
	{
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(Instance) * maxElements;	// �C���X�^���X�o�b�t�@�̃T�C�Y
		// GPU->�ǂݎ��̂݁@CPU->�������݂̂�
		bd.Usage = D3D11_USAGE_DYNAMIC;				// �o�b�t�@�̓ǂݏ������@
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// �o�b�t�@�̃p�C�v���C���ւ̃o�C���h����
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// ���\�[�X�ɋ������CPU�A�N�Z�X�̎w��
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = instances.get();	// ���_�̃A�h���X
		initData.SysMemPitch = 0;		//Not use for vertex buffers.mm
		initData.SysMemSlicePitch = 0;	//Not use for vertex buffers.
		hr = Systems::Device->CreateBuffer(&bd, &initData, instanceBuffer.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "failed create instance buffer dynamic(render_manager)");
			return;
		}
	}

	auto mat = Material_manager::create_material("default_material");

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

void Renderer_manager::render(std::list<Scenelist> active_scenes, const std::map<Scenelist, std::list<Camera_component*>>& cameras, const std::map<Scenelist, std::list<Light_component*>>& lights) {

	Work_meter::tag_start("render");

	// ���C����RTV��clear
	//Systems::Clear(); //�S���㏑�����邩�炵�Ȃ��Ă悢

	// Imgui��gpu�̏�����S���҂��₪�� & depth�ŏ㏑�������Ⴄ�̂� �O�t���[����texture��`�悵�� Imgui�̕`��������ōs��
	// camera->clear��gpu���ׂ�����AImgui�����̏�����҂̂� ���ɒu���Əd���Ȃ�
	{
		for (const auto& Sce : active_scenes) {
			if (cameras.count(Sce) == 0)continue;
			for (const auto& camera : cameras.at(Sce)) {
				// posteffect�̏��� & main��RTV�ɕ`��
				camera->posteffect_render();
			}
		}
	}

	// �O�p�`�̕`����@
	Systems::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//if (Sce == Scenelist::scene_null)return;

	// �o�b�`�p�Ɍ��݂̃V�[����mesh,material��sort���s��
	sort_update(active_scenes);

	for (const auto& Sce : active_scenes) {

		// light�����R���X�^���g�o�b�t�@�[�ɃZ�b�g����
		if (lights.count(Sce) != 0)
			set_light_Constantbuffer(lights.at(Sce));


		//���̃V�[���̃J�����̐�������
		if (cameras.count(Sce) != 0)
			for (const auto& camera : cameras.at(Sce)) {
				if (camera->gameobject->is_active == false)continue;

				const auto& frustum_data = camera->calculate_frustum_data();
				const auto& render_scene = camera->render_scene; //�J������render����scene


				// �J�����̑��݂���V�[�����J�������`�悷��V�[�����Ⴆ��update�����Ă��Ȃ��̂� update���Ă�
				//if (Sce != render_scene) sort_update(Sce);

				// �e�p�̕`��
				{
					camera->directional_shadow->setup();
					camera->directional_shadow->shader_activate();
					auto shadow_frustum = camera->directional_shadow->calculate_frustum_data();

					// instansing�p
					instance_update(shadow_frustum, render_scene); //frastum�ɂ���Ē������邽��

					render_instance(true, true);

					camera->directional_shadow->set_ShaderResourceView();
				}

				{
					Systems::SetViewPort(Al_Global::SCREEN_WIDTH, Al_Global::SCREEN_HEIGHT);

					// camera�̎���texture��clear
					camera->clear();

					// camera��������(constantbuffer,rendertargetview)
					camera->setup();

					Work_meter::start("render_obj");

					// instansing�p
					instance_update(frustum_data, render_scene);

					// render���Ă�
					render_instance(true);
					Work_meter::stop("render_obj");
				}

				// sky sphere
				{
					camera->sky_sphere->position = camera->transform->position + Vector3(0, -500, 0);
					camera->sky_sphere->rotate = Vector3(0, 180, 0);
					camera->sky_sphere->fov = camera->fov;
					camera->sky_sphere->aspect = camera->aspect;
					camera->sky_sphere->nearZ = camera->nearZ;
					camera->sky_sphere->farZ = 100000.0f;
					camera->sky_sphere->shader_activate();
					camera->sky_sphere->render();
				}

				// collider��render
				Physics_function::Physics_manager::render_collider();
			}
	}

	Work_meter::tag_stop();
}


// Queue�\�[�g�p
bool sortmesh(const Renderer_base* left, const Renderer_base* Right)
{
	return left->get_meshnum() < Right->get_meshnum();
}
bool sortmaterial(const Renderer_base* left, const Renderer_base* Right)
{
	return left->get_materialnum() < Right->get_materialnum();
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
				if (light->gameobject->is_active == false)return;
				PointLight[point_num] = *light->PointLight[o];
				//	PointLight[point_num].pos = (*itr_li->get()->PointLight[o]->pos )+( *itr_li->get()->transform->position);
				point_num++;
			}

			for (u_int o = 0; o < light->SpotLight.size(); o++) {
				if (light->gameobject->is_active == false)return;
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

void Renderer_manager::sort_update(std::list<Scenelist> active_scenes) {

	//if (renderers[Sce].size() == 0)return;
	int scene_size = 0;
	for (const auto& Sce : active_scenes) scene_size += renderers[Sce].size();

	sorted_renderers.clear();
	sorted_renderers.reserve(scene_size);

	for (const auto& Sce : active_scenes) {
		for (auto& renderer : renderers[Sce]) {
			sorted_renderers.emplace_back(renderer);
		}
	}

	// mesh��sort
	{
		auto begin = sorted_renderers.begin();
		auto end = sorted_renderers.end();
		// insert_sort���ǂ��Ǝv�����߂�ǂ������̂�std::sort���g�p����
		std::sort(begin, end, sortmesh);
	}

	// material�Ń\�[�g
	{
		auto itr = sorted_renderers.begin();
		auto itr_end = sorted_renderers.end();

		auto sort_itr_begin = itr;
		auto sort_itr_end = itr;

		long mesh_num = (*itr)->get_meshnum();
		for (itr = sorted_renderers.begin(); itr != itr_end; ++itr) {

			// ����mesh�łȂ��Ƃ�����������炻�͈̔͂�sort
			if ((*itr)->get_meshnum() != mesh_num) {
				sort_itr_end = itr;
				//--sort_itr_end;
				std::sort(sort_itr_begin, sort_itr_end, sortmaterial);

				sort_itr_begin = itr;
			}
		}

		// �Ō��mesh���͏�L��for��sort�Ɉ���������Ȃ��̂ł�����sort
		{
			sort_itr_end = itr;
			std::sort(sort_itr_begin, sort_itr_end, sortmaterial);
		}
	}
}

void Renderer_manager::instance_update(const Frustum_data& frustum_data, Scenelist Sce) {

	Instance* instances = nullptr;
	if (sorted_renderers.size() == 0)return;

	// instance_buffer��Map����
	HRESULT hr = S_OK;
	const D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	hr = Systems::DeviceContext->Map(instanceBuffer.Get(), 0, map, 0, &mappedBuffer);

	if (FAILED(hr))
	{
		assert(0 && "failed Map InstanceBuffer dynamic(RenderManager)");
		return;
	}
	instances = static_cast<Instance*>(mappedBuffer.pData);

	// renderer�̏�񂩂�
	{
		long mesh_num = sorted_renderers[0]->get_meshnum();
		long material_num = sorted_renderers[0]->get_materialnum();

		// instancing�`��̂��߂̔z���������
		render_counts.clear();

		Render_count render_count;
		render_count.renderer = sorted_renderers[0];
		render_count.bufferStart = 0;
		render_count.bufferCount = 0;

		std::vector<Renderer_base*> no_use_instancing_renderers;

		for (auto& renderer : sorted_renderers) {

			if (renderer->is_render == false)continue;
			// instancing ���s��Ȃ��ꍇ �ʂ̔z��ɕۑ�
			if (renderer->is_use_instancing == false) no_use_instancing_renderers.emplace_back(renderer);

			// �V����instance
			if (mesh_num != renderer->get_meshnum() ||
				material_num != renderer->get_materialnum()
				) {
				render_counts.emplace_back(render_count);

				render_count.bufferStart = render_count.bufferStart + render_count.bufferCount;
				render_count.bufferCount = 0;
				render_count.renderer = renderer;

				mesh_num = renderer->get_meshnum();
				material_num = renderer->get_materialnum();
			}

			// frustum�ɓ����Ă�����instance�ɒǉ�
			if (renderer->check_frustum(frustum_data)) {
				// count�𑝂₷
				renderer->set_instance(instances[render_count.bufferStart + render_count.bufferCount]);
				++render_count.bufferCount;
			}

		}
		// ��ԍŌ�̗v�f��for�O�Œǉ�����
		render_counts.emplace_back(render_count);

		// instancing ���s��Ȃ������ʂ�render_count�ɕۑ�
		{
			for (auto& renderer : no_use_instancing_renderers) {

				if (renderer->check_frustum(frustum_data)) {
					render_count.bufferStart = 0;
					render_count.bufferCount = 1;
					render_count.renderer = renderer;
					render_counts.emplace_back(render_count);
				}
			}
		}
	}

	// instance_buffer��Unmap����
	Systems::DeviceContext->Unmap(instanceBuffer.Get(), 0);

}

void Renderer_manager::render_instance(bool is_shader_activate, bool is_shadow_render) {
	//return;
	for (const auto& render : render_counts) {

		// �e�̕`����s��Ȃ����continue
		if (is_shadow_render && render.renderer->get_material() != nullptr) {
			if (render.renderer->get_material()->is_render_shadow == false)continue;
		}


		Work_meter::start("shader_activete");
		if (is_shader_activate)render.renderer->shader_activete();
		Work_meter::stop("shader_activete");

		Work_meter::start("render_instancing");
		render.renderer->render_instancing(instanceBuffer, render.bufferStart, render.bufferCount);
		Work_meter::start("render_instancing");
	}
}