#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"

class Player_base : public Component {
public:
	Collider* collider; //���g��collider�ւ̃|�C���^
	vector3* pos; //���g�̍��W
	vector3* g_pos; //�ڕW�̍��W
	float* r; //���g�̔��a
	std::vector<Player_base*> springs;

	float max_speed;
};