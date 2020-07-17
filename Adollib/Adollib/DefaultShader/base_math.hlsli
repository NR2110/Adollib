
#ifndef BASE_MATH_HLSLI
#define BASE_MATH_HLSLI

#define PI 3.14159265359f
#define inv_PI 0.31830988618f
#define inv_90 0.01111111111f
#define inv_180 0.00555555555f
#define inv_360 0.00277777777f

static float ToRadian(float Ang) {
	return Ang * PI * 0.00555555555;
}

static float ToAngle(float rad) {
	return rad * 180 * inv_PI;
}

static float atan2(float  y, float x) {
	float3 V1 = float3(x, y, 0);
	float3 V2 = float3(1, 0, 0);

	return acos(dot(V1, V2) / (length(V1) * length(V2)));
}

float3 matrix_to_euler(matrix M) {

	float3 R;
	if (M._32 >= 1.0f) {
		R.x = inv_PI;
		R.y = 0;
		R.z = atan2(M._21, M._11);
	}
	else if (M._32 <= -1.0f) {
		R.x = -inv_PI;
		R.y = 0;
		R.z = atan2(M._21, M._11);
	}
	else {
		R.x = asin(-M._32);
		R.y = atan2(M._31, M._33);
		R.z = atan2(M._12, M._22);
	}

	return R;
}

#endif

