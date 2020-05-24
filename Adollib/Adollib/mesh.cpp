#include "mesh.h"
#include "mesh_manager.h"

namespace Adollib {


	Mesh::Mesh(ID3D11Device* device, const char* fileName, const char* filePath)
	{
		HRESULT hr = S_OK;

		// FBXの読み込み
		Mesh_manager::CreateModelFromFBX(device, meshes, fileName, filePath);

		// 頂点シェーダー,入力レイアウトオブジェクトの生成 
#pragma region CreateVertexShader
		if (meshes[0].skeletalAnimation.size() > 0)
		{
			D3D11_INPUT_ELEMENT_DESC inputElemntDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONES"  ,  0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			Mesh_manager::CreateVsFromCso(device, "skinned_mesh_vs.cso", vertexShader.GetAddressOf(), vertexLayout.GetAddressOf(), inputElemntDesc, ARRAYSIZE(inputElemntDesc));
		}
		else
		{
			D3D11_INPUT_ELEMENT_DESC inputElemntDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			Mesh_manager::CreateVsFromCso(device, "skinned_mesh2_vs.cso", vertexShader.GetAddressOf(), vertexLayout.GetAddressOf(), inputElemntDesc, ARRAYSIZE(inputElemntDesc));
		}
#pragma endregion

		// テクスチャの生成
#pragma region CreateTexture
		D3D11_SAMPLER_DESC smpDesc;
		smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		smpDesc.MipLODBias = 0;
		smpDesc.MaxAnisotropy = 16;
		smpDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		smpDesc.MinLOD = 0;
		smpDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&smpDesc, samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#pragma endregion

		// ラスタライザーステートオブジェクトの生成（線描画・塗りつぶし描画） 
#pragma region Create RasterizerState 
		D3D11_RASTERIZER_DESC rasterizerDesc = {};

		// 線描画
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;   // レンダリング時の塗りつぶしモード
		rasterizerDesc.CullMode = D3D11_CULL_NONE;    // CullingMode
		rasterizerDesc.FrontCounterClockwise = TRUE; //三角形が表向きか裏向きか:FALSE->反時計方向が表 TRUE->時計方向が表
		rasterizerDesc.DepthBias = 0;                //特定のピクセルに追加される深度値
		rasterizerDesc.DepthBiasClamp = 0;            // ピクセルの最大深度バイアス
		rasterizerDesc.SlopeScaledDepthBias = 0;      //指定されたピクセルの傾斜のスカラー
		rasterizerDesc.DepthClipEnable = FALSE;	      // クリッピング有効にするかどうか
		rasterizerDesc.ScissorEnable = FALSE;	      // シザー長方形カリングを有効か
		rasterizerDesc.MultisampleEnable = FALSE;	  // MSAAを有効にするかどうか
		rasterizerDesc.AntialiasedLineEnable = FALSE; // ラインアンチエイリアシングを有効にするかどうか
		hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStateLine.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// 塗りつぶし描画
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;   // レンダリング時の塗りつぶしモード
		rasterizerDesc.CullMode = D3D11_CULL_NONE;    // CullingMode
		rasterizerDesc.FrontCounterClockwise = TRUE; //三角形が表向きか裏向きか:FALSE->反時計方向が表 TRUE->時計方向が表
		rasterizerDesc.DepthBias = 0;     //特定のピクセルに追加される深度値
		rasterizerDesc.DepthBiasClamp = 0;     // ピクセルの最大深度バイアス
		rasterizerDesc.SlopeScaledDepthBias = 0;     //指定されたピクセルの傾斜のスカラー
		rasterizerDesc.DepthClipEnable = FALSE; // クリッピング有効にするかどうか
		rasterizerDesc.ScissorEnable = FALSE; // シザー長方形カリングを有効か
		rasterizerDesc.MultisampleEnable = FALSE; // MSAAを有効にするかどうか
		rasterizerDesc.AntialiasedLineEnable = FALSE; // ラインアンチエイリアシングを有効にするかどうか
		hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStateFill.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#pragma endregion

		// 深度ステンシル ステート オブジェクトの生成 
#pragma region Create DepthStencilState
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE; // 深度テストを有効にするかどうか
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 深度ステンシルバッファへの書き込み設定
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;           // 比較オプション
		depthStencilDesc.StencilEnable = FALSE; // ステンシルテストを有効にするかどうか
		hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#pragma endregion

		// 定数バッファオブジェクトの生成
#pragma region Create ConstBuffer
		D3D11_BUFFER_DESC	constDesc = {};
		constDesc.ByteWidth = sizeof(ConBuffer) + 4;           // バッファのサイズ	
		constDesc.Usage = D3D11_USAGE_DEFAULT;	           // バッファの読み書き法
		constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;  // パイプラインにどうバインドするか指定
		constDesc.CPUAccessFlags = 0;                      // CPUのアクセスフラグ　0でアクセスしない
		constDesc.MiscFlags = 0;                           // その他のフラグ
		constDesc.StructureByteStride = 0;                 // バッファ構造体の場合の要素数
		hr = device->CreateBuffer(&constDesc, NULL, constantBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#pragma endregion

	}

	// 描画
	void AlcLib::Mesh::Render(
		ID3D11DeviceContext* context,
		const XMFLOAT4& lightDirect,
		float elapsed_time
	)
	{
		static float testShininess = 0;
		timer += elapsed_time;

		// 入力の取得
		static bool debugMode = true;
#pragma region Input
		if (debugMode)
		{
			int speed = 1;

			// 描画方法の切り替え
			//if (key.Space)
			if (GetKeyDown(Keyboard::Keys::Space))
			{
				isLineDraw = !isLineDraw;

				// font
				Print("change flag");
			}
			// 回転
			if (GetKey(Keyboard::Keys::LeftControl))
				speed = 3;
			if (GetKey(Keyboard::Keys::E))
				rot.y += 0.1f * speed;
			if (GetKey(Keyboard::Keys::Q))
				rot.y -= 0.1f * speed;
			if (GetKey(Keyboard::Keys::R))
			{
				pos.x = 0; pos.y = 0; pos.z = 0; rot.y = 0;
			}
			// アニメ
			if (GetKeyDown(Keyboard::Keys::T))
				animeIndex = animeIndex++ % 8;
			// テスト
			if (GetKey(Keyboard::Keys::L))
				testShininess += 0.1;
		}
#pragma endregion

		// 行列
#pragma region MATRIX 
		// ワールド行列 拡大*回転*移動
		{
			XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
			XMMATRIX R = XMMatrixRotationX(rot.x) * XMMatrixRotationY(rot.y) * XMMatrixRotationZ(rot.z);
			XMMATRIX T = XMMatrixTranslation(pos.x, pos.y, pos.z);

			XMStoreFloat4x4(&world, S * R * T);
		}
		// ワールド行列、ビュー行列、プロジェクション行列を合成し行列データを取り出す。
		{
			XMMATRIX WVP;
			WVP = XMLoadFloat4x4(&world) * XMLoadFloat4x4(&Camera::V) * XMLoadFloat4x4(&Camera::P);
			XMStoreFloat4x4(&worldViewProjection, WVP);
		}
#pragma endregion

		// 各設定のセット
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(vertexLayout.Get());

		context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
		if (isLineDraw == TRUE) context->RSSetState(rasterizerStateLine.Get());
		else context->RSSetState(rasterizerStateFill.Get());
		context->VSSetShader(vertexShader.Get(), NULL, 0);

		context->OMSetDepthStencilState(depthStencilState.Get(), 0);

		for (auto& mesh : meshes)
		{
			UINT stride = sizeof(VertexFormat);
			UINT offset = 0;
			context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
			context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			ConBuffer cb;
			{
				// WVP
				XMStoreFloat4x4(&cb.worldViewProjection,
					XMMatrixTranspose(
						XMLoadFloat4x4(&mesh.globalTransform) *
						/*XMLoadFloat4x4(&coordinate_conversion) **/
						XMLoadFloat4x4(&worldViewProjection)));
				// World
				XMStoreFloat4x4(&cb.world,
					XMLoadFloat4x4(&mesh.globalTransform) *
					/*XMLoadFloat4x4(&coordinate_conversion) **/
					XMLoadFloat4x4(&world));
				// boneTransform
				if (mesh.skeletalAnimation.size() > 0 && mesh.skeletalAnimation[animeIndex].size() > 0)
				{
					int frame = mesh.skeletalAnimation.at(animeIndex).animation_tick / mesh.skeletalAnimation.at(animeIndex).sampling_time;
					if (frame > mesh.skeletalAnimation.at(animeIndex).size() - 1)
					{
						frame = 0;
						mesh.skeletalAnimation.at(animeIndex).animation_tick = 0;
					}
					vector<bone>& skeletal = mesh.skeletalAnimation.at(animeIndex).at(frame);
					size_t number_of_bones = skeletal.size();
					_ASSERT_EXPR(number_of_bones < MAX_BONES, L"'the number_of_bones' exceeds MAX_BONES.");
					for (size_t i = 0; i < number_of_bones; i++)
					{
						XMStoreFloat4x4(&cb.boneTransforms[i], XMLoadFloat4x4(&skeletal.at(i).transform));
					}
					mesh.skeletalAnimation.at(animeIndex).animation_tick += elapsed_time;
				}
				// specular
				XMFLOAT3 cameraPos = Camera::GetInstance()->GetPos();
				cb.cameraWPos = XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0);
				cb.specular = XMFLOAT4(1, 1, 1, 1.0);
				cb.shininess = 1;
				// elapsedTime
				cb.time = timer;
				// ambient
				cb.ambientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
				// material
				cb.materialColor = XMFLOAT4(1, 1, 1, 1);
				// light
				cb.lightDirection = lightDirect;
				cb.lightColor = XMFLOAT4(1, 1, 1, 1);
				// emission
				cb.emmisionColor = XMFLOAT4(0, 0, 0, 1);
				cb.intensity = 1;
			}
			context->UpdateSubresource(constantBuffer.Get(), 0, NULL, &cb, 0, 0);
			context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
			context->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			for (auto& subset : mesh.subsets)
			{
				context->PSSetShaderResources(0, 1, subset.diffuse.shaderResourceVirw.GetAddressOf());
				context->PSSetShader(subset.pixelShader.Get(), NULL, 0);

				// 描画
				context->DrawIndexed(subset.indexCount, subset.indexStart, 0);

			}
		}
	}




	// FBXから頂点インデックスを取得する
	// FbxMesh         取得するFBX
	// vector<u_int>*  保存したい頂点インデックス
	void Mesh::CreateIndices(FbxMesh* mesh, vector<u_int>* indices)
	{
		// 総ポリゴン数
		int polygonNum = mesh->GetPolygonCount();

		//indices->assign(vertexCnt, -1);
		static int index = 0;

		for (int p = 0; p < polygonNum; p++) {
			for (int n = 0; n < 3; n++)
			{
				indices->at(p * 3 + n) = mesh->GetPolygonVertex(p, n);
				index++;
			}
		}
	}




}