#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"

class Stage_base : public Component {
public:
	Collider* collider; //���g��collider�ւ̃|�C���^

	vector3 pos;
	vector3 scale;
	quaternion quat;

	vector3* camera_pos;
	quaternion* camera_orient;

};
