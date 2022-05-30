#pragma once

#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Object/component.h"

namespace Adollib {

	class Simulation_GPUparticle : public Component {

	private:
		int cs_thread = 512;

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