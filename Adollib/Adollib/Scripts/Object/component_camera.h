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

	//カメラ用のコンポーネントクラス 継承不可!
	class Camera_component final : public Component
	{
	public:
		float fov = 60.0f;
		float aspect = 1280 / 720.0f;
		float nearZ = 0.1f;
		float farZ = 10000.0f;
		Vector4 clear_color = Vector4(0, 0, 0, 0);

		// 画面に出力するか
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

		// managerに保存されている 自身へのitr
		std::list<Camera_component*>::iterator this_itr;

	private:
		// addされたposteffectを初期化する ヘッダーでできないため関数にした
		void posteffect_initialize(Posteffect_base* posteffect);

	public:
		std::shared_ptr<Texture> get_color_texture() { return color_texture; };

	public:
		// redertargetviewなどをclearする
		void clear();

		// ConstantBufferにcamera情報をsetする
		void set_Constantbuffer();

		// ポストエフェクトの処理 & 描画
		void posteffect_render();

		// ポストエフェクトの追加
		template<typename T>
		T* add_posteffect() {

			// Posteffect_baseクラスから派生したものかチェック
			static_assert(std::is_base_of<Posteffect_base, T>::value == true, "template T must inherit Posteffect_base");

			// 既に同一posteffectが存在する場合、Addしない
			for (auto&& com : posteffects)
			{
				//if (dynamic_cast<T*>(com) != nullptr)
				if (typeid(T) == typeid(*com))
				{
					//static_assert(0, "ERROR");
					return nullptr;
				}
			}

			// 生成
			T* newCom = newD T();

			// 初期化
			posteffect_initialize(newCom);

			return newCom;
		};

	public:

		// addComponentされたときに呼ばれる
		void awake() override;

		// Hierarchyの表示(Imguiの関数 Imgui::begin,Imgui::endはいらない)
		virtual void Update_hierarchy() override;

		// removeComponent()、clear()時に呼ぶ
		void finalize() override;

	};



};
