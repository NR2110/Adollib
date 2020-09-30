#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"

class Player_base : public Component {
public:
	Collider* collider; //自身のcolliderへのポインタ
	vector3* pos; //自身の座標
	vector3* g_pos; //目標の座標
	float* r; //自身の半径
	std::vector<Player_base*> springs;

	float max_speed;
};