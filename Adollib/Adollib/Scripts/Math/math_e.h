#pragma once

#define USE_SIMD 0

namespace Adollib {

	static float ToRadian(float angle) {
		return (float)angle * (3.141592654f / 180.0f);
	}

	static float ToAngle(float radian) {
		return (float)radian * 180 / 3.1415926535f;
	}

#define ALmax(a,b) (a>b?a:b)
#define ALmin(a,b) (a<b?a:b)
#define ALClamp(a,min_,max_) (ALmin(ALmax(a,min_),max_))

	typedef unsigned char   u_char;
	typedef unsigned short  u_short;
	typedef unsigned int    u_int;
	typedef unsigned long   u_long;

}
