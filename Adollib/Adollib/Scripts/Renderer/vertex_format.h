#pragma once
#include "../Math/math.h"

namespace Adollib{
	// 頂点フォーマット
	struct VertexFormat
	{
	#define MAX_BONE_INFLUENCES (4)
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
		float bone_weights[MAX_BONE_INFLUENCES] = { 0 };
		int bone_indices[MAX_BONE_INFLUENCES] = {};
	};

	//struct VertexFormat
	//{
	//	DirectX::XMFLOAT3 position;
	//	DirectX::XMFLOAT3 normal;
	//	DirectX::XMFLOAT2 texcoord;
	//};

}