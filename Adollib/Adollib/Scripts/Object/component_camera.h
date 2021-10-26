#pragma once

#include "Object.h"
#include "component.h"
#include "time.h"
#include "../Main/input.h"
#include "../Renderer/Shader/shader.h"
#include "../Scene/scene_list.h"

#include <string>
#include <memory>
#include <d3d11.h>
#include <wrl.h>

namespace Adollib
{
	class Texture;
	class Posteffect_base;

	//�J�����p�̃R���|�[�l���g�N���X �p���s��!
	class Camera_component final : public Component
	{
	public:
		float fov = 60.0f;
		float aspect = 1280 / 720.0f;
		float nearZ = 0.1f;
		float farZ = 10000.0f;
		Vector4 clear_color = Vector4(0, 0, 0, 0);

		// ��ʂɏo�͂��邩
		bool is_draw_main_RenderTargetView = true;

		Scenelist render_scene = Scenelist::scene_null;

	private:
		std::list<Posteffect_base*> posteffects;

		Microsoft::WRL::ComPtr<ID3D11Buffer> view_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> projection_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> sprite_cb;

		std::shared_ptr<Texture> color_texture = nullptr;
		std::shared_ptr<Texture> normal_texture = nullptr;
		std::shared_ptr<Texture> depth_texture = nullptr;

		Shader shader;


	private:

		// manager�ɕۑ�����Ă��� ���g�ւ�itr
		std::list<Camera_component*>::iterator this_itr;

	private:
		// add���ꂽposteffect������������ �w�b�_�[�łł��Ȃ����ߊ֐��ɂ���
		void posteffect_initialize(Posteffect_base* posteffect);

	public:
		std::shared_ptr<Texture> get_color_texture() { return color_texture; };

	public:
		// redertargetview�Ȃǂ�clear����
		void clear();

		// ConstantBuffer��camera����set����
		void set_Constantbuffer();

		// �|�X�g�G�t�F�N�g�̏��� & �`��
		void posteffect_render();

		// �|�X�g�G�t�F�N�g�̒ǉ�
		template<typename T>
		T* add_posteffect() {

			// Posteffect_base�N���X����h���������̂��`�F�b�N
			static_assert(std::is_base_of<Posteffect_base, T>::value == true, "template T must inherit Posteffect_base");

			// ���ɓ���posteffect�����݂���ꍇ�AAdd���Ȃ�
			for (auto&& com : posteffects)
			{
				//if (dynamic_cast<T*>(com) != nullptr)
				if (typeid(T) == typeid(*com))
				{
					//static_assert(0, "ERROR");
					return nullptr;
				}
			}

			// ����
			T* newCom = newD T();

			// ������
			posteffect_initialize(newCom);

			return newCom;
		};

	public:

		// addComponent���ꂽ�Ƃ��ɌĂ΂��
		void awake() override;

		// Hierarchy�̕\��(Imgui�̊֐� Imgui::begin,Imgui::end�͂���Ȃ�)
		virtual void Update_hierarchy() override;

		// removeComponent()�Aclear()���ɌĂ�
		void finalize() override;

	};



};
