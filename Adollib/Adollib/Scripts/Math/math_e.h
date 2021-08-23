#pragma once

#define USE_SIMD 0

namespace Adollib {

	static float ToRadian(float angle) {
		constexpr float value= (3.141592654f / 180.0f);
		return (float)angle * value;
	}

	static float ToAngle(float radian) {
		constexpr float value = (180 / 3.1415926535f);
		return (float)radian * value;
	}

	constexpr float inv120(0.00833333f);
	constexpr float inv90(0.011111111f);
	constexpr float inv60(0.016666666f);
	constexpr float inv30(0.033333333f);
	constexpr float PI(3.14159265358979f);
	constexpr float invPI(0.318309886f);
	constexpr float inv2PI(0.159154943f);
	constexpr float PI_inv2(1.570796327f);
	constexpr float PI_inv4(0.785398163f);

#define ALmax(a,b) (a>b?a:b)
#define ALmax3(a,b,c) (ALmax(ALmax(a,b),c))
#define ALmin(a,b) (a<b?a:b)
#define ALmin3(a,b,c) (ALmin(ALmin(a,b),c))

#define ALClamp(a,min_,max_) (ALmin(ALmax(a,min_),max_))
#define ALCover(a,b,c) (((a - b) * (c - b)) < 0)
#define ALEasing(start,goal,speed,delta_time) goal + (start - goal) * pow(1 - speed, delta_time * 60)

	typedef unsigned char   u_char;
	typedef unsigned short  u_short;
	typedef unsigned int    u_int;
	typedef unsigned long   u_long;

}
