#pragma once

#include <vector>
#include <string>
#include <list>
#include <map>
#include <unordered_map>

#include "../Scene/scene_list.h"
#include "instance.h"
#include "frustum_data.h"
#include "mesh.h"
#include "material.h"


namespace Adollib {

	class Camera_component;
	class Light_component;
	class Renderer_base;


	class Renderer_manager {

	private:
		static Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer;
		static std::vector<Renderer_base*> sorted_renderers; //instansing�̂��߂Ƀ\�[�g���ꂽ�f�[�^ vector�ɂ���������renderers�Ƃ͕ʂɊǗ�
		static std::vector<Render_count> render_counts; //instancing�`��̂��߂̔z�� ������for�ŉ񂵂ĕ`����s��

		static std::unordered_map<Scenelist, std::list<Renderer_base*>> renderers;

		// light�͑S���܂Ƃ߂�cb��set���邽�� lightcomponent�ł͏����ł��Ȃ�
		static Microsoft::WRL::ComPtr<ID3D11Buffer> light_cb;

	public:
		// renderer::component���A�^�b�`���ꂽ�Ƃ� �R���X�g���N�^�ŌĂ΂��
		static std::list<Renderer_base*>::iterator add_renderer(Renderer_base* renderer);

		// renderer::component���폜�����Ƃ��Ă΂��
		static void remove_renderer(std::list<Renderer_base*>::iterator itr);

	private:
		static void set_light_Constantbuffer(const std::list<Light_component*>& lights);

		static void sort_update(std::list<Scenelist> active_scenes); //instance�̂��߂�sort material��mesh������renderer��A�Ԃɂ���sort���s��

		static void instance_update(const Frustum_data& frustum_data, Camera_component* camera_comp); //sort_update�ł܂Ƃ߂���񂩂� instancing�̉񐔂Ȃǂ��v�Z�Arender_counts�ɕۑ�

		static void render_instance(Camera_component* camera_comp, bool is_PSshader_update = true, bool is_shadow_render = false);

	public:
		static void awake();

		static void render(const std::list<Scenelist>& active_scenes, const std::map<Scenelist, std::list<Camera_component*>>& cameras, const std::map<Scenelist, std::list<Light_component*>>& lights);

	};


}