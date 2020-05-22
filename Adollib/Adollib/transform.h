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
		vector3 position;
		quaternion orientte;
		vector3 scale;

		//親からの相対情報
		vector3 local_pos;
		quaternion local_orient;
		vector3 local_scale;

		// アタッチされたいるcollider
		std::list<std::shared_ptr<RigitBody_Transform>> colliders;
	};

	//衝突用transfome
	class RigitBody_Transform {

		std::shared_ptr<Transfome> pearent; //このcolliderがアタッチされたTransfomeへのポインタ

		Rigitbody* rigitbody; //collider本体

	};
}