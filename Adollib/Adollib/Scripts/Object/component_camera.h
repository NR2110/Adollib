#pragma once

#include "Object.h"
#include "component.h"
#include "time.h"
#include "../Main/input.h"
#include "../Renderer/Shader/shader.h"

#include <string>
#include <memory>
#include <d3d11.h>
#include <wrl.h>

namespace Adollib
{
	class Texture;

	//カメラ用のコンポーネントクラス 継承不可!
	class Camera_component final : public Component
	{
	public:
		float fov = 60.0f;
		float aspect = 1280 / 720.0f;
		float nearZ = 0.1f;
		float farZ = 10000.0f;


	private:
		// managerに保存されている 自身へのitr
		std::list<Camera_component*>::iterator this_itr;

		Microsoft::WRL::ComPtr<ID3D11Buffer> view_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> projection_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> sprite_cb;

		Texture* color_texture = nullptr;
		Texture* normal_texture = nullptr;
		Texture* depth_texture = nullptr;

		Shader shader;

	private:
		void clear();

	public:
		// ConstantBufferにcamera情報をsetする
		void set_Constantbuffer();

		// それぞれのTextureを表示する
		void MRT_render();

	public:

		// addComponentされたときに呼ばれる
		void awake() override;

		// Hierarchyの表示(Imguiの関数 Imgui::begin,Imgui::endはいらない)
		virtual void Update_hierarchy() override;

		// removeComponent()、clear()時に呼ぶ
		void finalize() override;

	};



};
