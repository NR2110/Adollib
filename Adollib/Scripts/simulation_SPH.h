#pragma once

#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Object/component.h"

namespace Adollib {


	//struct Particle{
	//	Vector3 pos; //座標
	//	Vector3 velocity; //速度
	//	Vector3 force; //加速度

	//	float pressure; //圧力
	//	float stress; //応力
	//	float strain; //ひずみ

	//	//:: 定数
	//	float density; //密度
	//	float young_modulus; //ヤング率
	//	float Poisson_ratio; //ポアソン比

	//	Gameobject* gameobject = nullptr; //描画用
	//};


	class Simulation_SPH : public Component{

	private:

		//std::vector<Particle> particles; //描画用



	private:
		//void simulation(std::vector<Particle>* p_ps);
		//void calculate_density_and_pressure(std::vector<Particle>* p_ps);
		//void calculate_force(std::vector<Particle>* p_ps);
		//void calculate_position(std::vector<Particle>* p_ps);

	public:
		void awake() override;

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start() override {};

		// 毎フレーム呼ばれる更新処理
		void update() override;

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable() override {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable() override {};

	};


}