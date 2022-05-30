#pragma once

#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Object/component.h"

namespace Adollib {

	class Simulation_GPUparticle : public Component {

	private:
		int cs_thread = 512;

		//std::vector<Particle> particles; //�`��p



	private:
		//void simulation(std::vector<Particle>* p_ps);
		//void calculate_density_and_pressure(std::vector<Particle>* p_ps);
		//void calculate_force(std::vector<Particle>* p_ps);
		//void calculate_position(std::vector<Particle>* p_ps);

	public:
		void awake() override;

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start() override {};

		// ���t���[���Ă΂��X�V����
		void update() override;

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable() override {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable() override {};

	};


}