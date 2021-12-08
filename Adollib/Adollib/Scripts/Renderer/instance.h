#pragma once

#include "../Math/math.h"

namespace Adollib {
	struct Instance
	{
		DirectX::XMFLOAT4X4 transformMatrix = {};	// world変換やNDC変換を入れる行列
		DirectX::XMFLOAT4 texcoordTransform = { 0.0f,0.0f,1.0f,1.0f };
		DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
	};

	// 描画スタート、カウントをまとめた構造体
	class Renderer_base;
	struct Render_count
	{
		int bufferStart = 0;			// インスタンスバッファ内の描画開始位置
		int bufferCount = 1;			// インスタンスバッファ内のスタートからの描画数
		Renderer_base* renderer = nullptr;
	};
}