#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"

class Stage_base : public Component {
public:
	Collider* collider; //自身のcolliderへのポインタ

	vector3 pos;
	vector3 scale;
	quaternion quat;

	vector3* camera_pos;
	quaternion* camera_orient;

};
