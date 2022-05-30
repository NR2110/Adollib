#pragma once

#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Object/component.h"

namespace Adollib {


	//struct Particle{
	//	Vector3 pos; //���W
	//	Vector3 velocity; //���x
	//	Vector3 force; //�����x

	//	float pressure; //����
	//	float stress; //����
	//	float strain; //�Ђ���

	//	//:: �萔
	//	float density; //���x
	//	float young_modulus; //�����O��
	//	float Poisson_ratio; //�|�A�\����

	//	Gameobject* gameobject = nullptr; //�`��p
	//};
	namespace Compute_S {
		class ComputeShader;
	}

	class Simulation_SPH : public Component{

	private:
		struct ConstantBuffer_particle {
			int _NumParticles;        // ���q��
			float _TimeStep;          // ���ԍ��ݕ�(dt)
			float _Smoothlen;         // ���q���a
			float _PressureStiffness; // Becker�̌W��
			float _RestDensity;       // �Î~���x
			float _DensityCoef;       // ���x�Z�o���̌W��
			float _GradPressureCoef;  // ���͎Z�o���̌W��
			float _LapViscosityCoef;  // �S���Z�o���̌W��
			float _WallStiffness;     // �y�i���e�B�@�̉����Ԃ���
			float _Viscosity;         // �S���W��
			float dummy_0;         // �S���W��
			float dummy_1;         // �S���W��
			float dummy_2;         // �S���W��
			Vector3 _Gravity;          // �d��
		};
		struct ConstantBuffer_particlerange {
			Vector3 _Range_s;            // �V�~�����[�V�������
			float dummy_0;
			Vector3 _Range_g;            // �V�~�����[�V�������
			float dummy_1;
			Vector3 _Range_noraml_0;            // �V�~�����[�V�������
			float dummy_2;
			Vector3 _Range_noraml_1;            // �V�~�����[�V�������
			float dummy_3;
			Vector3 _Range_noraml_2;            // �V�~�����[�V�������
			float dummy_4;
		};

		struct Particle {
			Vector3 position;
			Vector3 velocity;
		};

		struct ParticlePressure {
			float pressure;
		};

		struct ParticleForces {
			Vector3 acceleration;
		};

		struct ParticleDensity {
			float density;
		};

	private:
		int nP;
		int count;
		int cs_thread = 512;

		float rotate = 0;

		std::vector<Particle> particles;
		std::vector<Gameobject*> particle_gos;

		std::shared_ptr<Compute_S::ComputeShader> density_CS;
		std::shared_ptr<Compute_S::ComputeShader> pressure_CS;
		std::shared_ptr<Compute_S::ComputeShader> collide_CS;
		std::shared_ptr<Compute_S::ComputeShader> force_CS;
		std::shared_ptr<Compute_S::ComputeShader> integrate_CS;

		// bufffer
		Microsoft::WRL::ComPtr<ID3D11Buffer> particle_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> dencity_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pressure_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> force_buffer;

		Microsoft::WRL::ComPtr<ID3D11Buffer> read_particle_buffer;

		Microsoft::WRL::ComPtr<ID3D11Buffer> particle_cb; //constantbuffer�p
		Microsoft::WRL::ComPtr<ID3D11Buffer> particlerange_cb; //constantbuffer�p

		// SRV,UAV
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particle_read;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dencity_read;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pressure_read;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> force_read;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_write;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> dencity_write;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pressure_write;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> force_write;
		//std::vector<Particle> particles; //�`��p

	private:
		void compute_shader_init();
		void RdDat();

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