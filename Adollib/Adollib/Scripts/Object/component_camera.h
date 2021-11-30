#pragma once

#include "Object.h"
#include "component.h"
#include "time.h"
#include "../Main/input.h"
#include "../Renderer/Shader/shader.h"
#include "../Renderer/frustum_data.h"
#include "../Scene/scene_list.h"

#include <string>
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <DirectXCollision.h>

namespace Adollib
{
	class Texture;
	class Posteffect_base;
	class Directional_shadow;

	//カメラ用のコンポーネントクラス 継承不可!
	class Camera_component final : public Component
	{
	public:
		float fov = 60.0f;
		float aspect = 1; //awakeで計算している
		float nearZ = 0.1f;
		float farZ = 10000.0f;
		//Vector4 clear_color = Vector4(0, 0, 0, 0);
		Vector4 clear_color = Vector4(0, 0.5f, 0.5f, 1);

		// 画面に出力するか
		bool is_draw_main_RenderTargetView = true;

		// 描画を行うscene 変更しなければアタッチした時のsceneになっている
		Scenelist render_scene = Scenelist::scene_null;

		// 影のptr
		std::shared_ptr<Directional_shadow> directional_shadow = nullptr;

	private:
		std::list<std::shared_ptr<Posteffect_base>> posteffects; //ポストエフェクト関係の保存

		Microsoft::WRL::ComPtr<ID3D11Buffer> view_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> projection_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> sprite_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> shadow_viewprojection_cb;

		std::shared_ptr<Texture> color_texture  = nullptr;
		std::shared_ptr<Texture> normal_texture = nullptr;
		std::shared_ptr<Texture> depth_texture  = nullptr;

		Shader shader;

	private:
		// managerに保存されている 自身へのitr
		std::list<Camera_component*>::iterator this_itr;

	private:
		// addされたposteffectを初期化する ヘッダーでできないため関数にした
		void posteffect_initialize(std::shared_ptr<Posteffect_base>);

	public:
		std::shared_ptr<Texture> get_color_texture() { return color_texture; };

	public:
		// redertargetviewなどをclearする
		void clear();

		// ConstantBufferにcamera情報をset, RenderetrgetViewの変更
		void setup();

		// ポストエフェクトの処理 & 描画
		void posteffect_render();

		// frustum情報の計算
		Frustum_data calculate_frustum_data();

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
