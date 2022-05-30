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
	namespace Compute_S {
		class ComputeShader;
	}

	class Simulation_SPH : public Component{

	private:
		struct ConstantBuffer_particle {
			int _NumParticles;        // 粒子数
			float _TimeStep;          // 時間刻み幅(dt)
			float _Smoothlen;         // 粒子半径
			float _PressureStiffness; // Beckerの係数
			float _RestDensity;       // 静止密度
			float _DensityCoef;       // 密度算出時の係数
			float _GradPressureCoef;  // 圧力算出時の係数
			float _LapViscosityCoef;  // 粘性算出時の係数
			float _WallStiffness;     // ペナルティ法の押し返す力
			float _Viscosity;         // 粘性係数
			float dummy_0;         // 粘性係数
			float dummy_1;         // 粘性係数
			float dummy_2;         // 粘性係数
			Vector3 _Gravity;          // 重力
		};
		struct ConstantBuffer_particlerange {
			Vector3 _Range_s;            // シミュレーション空間
			float dummy_0;
			Vector3 _Range_g;            // シミュレーション空間
			float dummy_1;
			Vector3 _Range_noraml_0;            // シミュレーション空間
			float dummy_2;
			Vector3 _Range_noraml_1;            // シミュレーション空間
			float dummy_3;
			Vector3 _Range_noraml_2;            // シミュレーション空間
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

		Microsoft::WRL::ComPtr<ID3D11Buffer> particle_cb; //constantbuffer用
		Microsoft::WRL::ComPtr<ID3D11Buffer> particlerange_cb; //constantbuffer用

		// SRV,UAV
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particle_read;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dencity_read;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pressure_read;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> force_read;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_write;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> dencity_write;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pressure_write;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> force_write;
		//std::vector<Particle> particles; //描画用

	private:
		void compute_shader_init();
		void RdDat();

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