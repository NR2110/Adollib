#pragma once
#include <DirectXMath.h>

struct Frustum_data {
	DirectX::XMVECTOR NearPlane;
	DirectX::XMVECTOR FarPlane;
	DirectX::XMVECTOR RightPlane;
	DirectX::XMVECTOR LeftPlane;
	DirectX::XMVECTOR TopPlane;
	DirectX::XMVECTOR BottomPlane;
};
