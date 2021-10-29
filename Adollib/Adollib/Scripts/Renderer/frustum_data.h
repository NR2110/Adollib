#pragma once
#include <DirectXMath.h>
#include "../Math/math.h"

struct Frustum_data {
	Adollib::Vector3 normals[6]; //各平面の法線
	float distances[6]; //各平面の距離
};
