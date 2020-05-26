#pragma once

#include "vector.h"
#include "quaternion.h"
#include "rigit_body.h"

#include <list>
#include <memory>

namespace Adollib {

	class RigitBody_Transform;

	//描画用transfome
	class Transfome {
	public:
		//描画用の情報 
		vector3 position = vector3(0, 0, 0);
		quaternion prientation = quaternion(1, 0, 0, 0);
		vector3 scale = vector3(1, 1, 1);

		//親からの相対情報
		vector3 local_pos = vector3(0, 0, 0);
		quaternion local_orient = quaternion(1, 0, 0, 0);
		vector3 local_scale = vector3(1, 1, 1);

		// アタッチされたいるcollider
		std::list<std::shared_ptr<RigitBody_Transform>> colliders;
	};

	//衝突用transfome
	class RigitBody_Transform {

		std::shared_ptr<Transfome> pearent; //このcolliderがアタッチされたTransfomeへのポインタ

		Rigitbody* rigitbody; //collider本体

	};
}