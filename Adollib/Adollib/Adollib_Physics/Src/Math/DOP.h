#pragma once
#include "vector.h"

namespace Adollib {
	namespace Physics_function {
		namespace DOP {
			static const int DOP_size = 6 / 2;

			//::::::::::::::::::
			//Boardphase用のDOP_14
			// x : (1,0,0)
			// y : (0,1,0)
			// z : (0,0,1)
			// a : (+0.3,0.3,+0.3)
			// b : (-0.3,0.3,+0.3)
			// c : (+0.3,0.3,-0.3)
			// d : (-0.3,0.3,-0.3)
			//:::::::::::::::::::
			static const Vector3 DOP_14_axis[DOP_size] = {
				Vector3(1,0,0),
				Vector3(0,1,0),
				Vector3(0,0,1)/*,
				Vector3(+0.577350269f,0.577350269f,+0.577350269f),
				Vector3(-0.577350269f,0.577350269f,+0.577350269f),
				Vector3(+0.577350269f,0.577350269f,-0.577350269f),
				Vector3(-0.577350269f,0.577350269f,-0.577350269f)*/
			};
			static const DirectX::XMVECTOR DOP_14_axis_SIM[DOP_size] = {
				DirectX::XMVectorSet(1,0,0,0),
				DirectX::XMVectorSet(0,1,0,0),
				DirectX::XMVectorSet(0,0,1,0)/*,
				Vector3(+0.577350269f,0.577350269f,+0.577350269f),
				Vector3(-0.577350269f,0.577350269f,+0.577350269f),
				Vector3(+0.577350269f,0.577350269f,-0.577350269f),
				Vector3(-0.577350269f,0.577350269f,-0.577350269f)*/
			};
			struct DOP_14 {
				//中心座標
				Vector3 pos;

				//::::::::::::::::::
				//Boardphase用のDOP
				// x : (1,0,0)
				// y : (0,1,0)
				// z : (0,0,1)
				// a : (1,1,1)
				// b : (-1,1,1)
				// c : (1,1,-1)
				// d : (-1,1,-1)
				//:::::::::::::::::::
				float max[DOP_size] = { 0 };

				//::::::::::::::::::
				//Boardphase用のDOP
				// x : (1,0,0)
				// y : (0,1,0)
				// z : (0,0,1)
				// a : (1,1,1)
				// b : (-1,1,1)
				// c : (1,1,-1)
				// d : (-1,1,-1)
				//:::::::::::::::::::
				float min[DOP_size] = { 0 };
			};


			static const Vector3 AABB_axis[3]{
				Vector3(1,0,0),
				Vector3(0,1,0),
				Vector3(0,0,1)
			};
			struct AABB {
				//中心座標
				Vector3 pos = Vector3(0);

				//+ - 方向への大きさ
				Vector3 max = Vector3(-FLT_MAX);
				Vector3 min = Vector3(+FLT_MAX);;
			};
		}
	}
}