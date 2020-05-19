#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <d3d11.h>
#include <dxgi.h>
#include <map>
#include <vector>

#pragma comment(lib,"d3d11.lib")

#include "quaternion.h"

namespace Adollib {
	class cameras {
	private:
	//	quaternion position;
	//	quaternion vect;
	//	DirectX::XMFLOAT4X4 fSpin;

	//public:
	//	static int camera_num;

	//	cameras();
	//	~cameras() {};
	//	void update();

	//	void set_pos(quaternion Q) { position = Q; };
	//	void set_pos(float x, float y, float z) { set_pos(quaternion(x, y, z)); };
	//	void set_vect(quaternion Q) { vect = Q; };
	//	void set_vect(float x, float y, float z) { set_vect(quaternion(x, y, z)); };
	//	void set_angle(float x, float y, float z) { vect.rotate(x, y, z); };

	//	const quaternion get_pos() const { return position; };
	//	const quaternion get_vect() const { return vect; };
	};
}

