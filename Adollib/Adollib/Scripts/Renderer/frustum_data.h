#pragma once
#include <DirectXMath.h>
#include "../Math/math.h"

struct Frustum_data {
	Adollib::Vector3 normals[6]; //�e���ʂ̖@��
	float distances[6]; //�e���ʂ̋���
};
