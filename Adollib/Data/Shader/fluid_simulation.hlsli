// 簡略化のためにスレッド数は1次元にしておく
//#define THREAD_SIZE_X 1024
#define THREAD_SIZE_X 512

// ▼ 構造化バッファの定義 ---------------------
struct Particle
{
    float3 position;
    float3 velocity;
};

struct ParticlePressure
{
    float pressure;
};

struct ParticleForces
{
    float3 acceleration;
};

struct ParticleDensity
{
    float density;
};

//// 粒子位置と速度を保持するバッファ
//StructuredBuffer<Particle> _ParticlesBufferRead                    : register(t0);
//RWStructuredBuffer<Particle> _ParticlesBufferWrite                 : register(u0);

//// 粒子密度を保持するバッファ
//StructuredBuffer<ParticleDensity> _ParticlesDensityBufferRead      : register(t1);
//RWStructuredBuffer<ParticleDensity> _ParticlesDensityBufferWrite   : register(u1);

//// 粒子圧力を保持するバッファ
//StructuredBuffer<ParticlePressure> _ParticlesPressureBufferRead    : register(t2);
//RWStructuredBuffer<ParticlePressure> _ParticlesPressureBufferWrite : register(u2);

//// 粒子加速度(力)を保持するバッファ
//StructuredBuffer<ParticleForces> _ParticlesForceBufferRead         : register(t3);
//RWStructuredBuffer<ParticleForces> _ParticlesForceBufferWrite      : register(u3);
// ▲ 構造化バッファの定義 ---------------------

// ▼ シェーダ定数の定義 -----------------------
//// ▼ シェーダ定数の定義 -----------------------
//cbuffer CB : register(b0)
//{
//    int _NumParticles; // 粒子数
//    float _TimeStep; // 時間刻み幅(dt)
//    float _Smoothlen; // 粒子半径
//    float _PressureStiffness; // Beckerの係数
//    float _n0; // 粒子数密度の基準値
//    float _lamd; // λ
//    float _DensityCoef; // 密度算出時の係数
//    float _GradPressureCoef; // 圧力算出時の係数
//    float _LapViscosityCoef; // 粘性算出時の係数
//    float _Viscosity; // 粘性係数

//    float3 _Range; // シミュレーション空間
//    float _WallStiffness; // ペナルティ法の押し返す力
//    float3 _Gravity; // 重力
//};
cbuffer CB : register(b0)
{
    int _NumParticles;        // 粒子数
    float _TimeStep;          // 時間刻み幅(dt)
    float _Smoothlen;         // 粒子半径
    float _PressureStiffness; // Beckerの係数
    float _RestDensity;       // 静止粒子数密度
    float _DensityCoef;       // Weight計算時の定数
    float _GradPressureCoef;  // 圧力算出時の係数
    float _LapViscosityCoef;  // 粘性算出時の係数
    float _WallStiffness;     // ペナルティ法の押し返す力
    float _Viscosity;         // 粘性係数
    float _dummy_01;
    float _dummy_02;
    float _dummy_03;
    float3 _Gravity;          // 重力
};

cbuffer CB_1 : register(b1)
{
    float3 _Range_start; // シミュレーション空間
    float _dummy_04;
    float3 _Range_end; // シミュレーション空間
    float _dummy_05;
    float3 _normal_01; // 法線
    float _dummy_06;
    float3 _normal_02; // 法線
    float _dummy_07;
    float3 _normal_03; // 法線
    float _dummy_08;
};
// ▲ シェーダ定数の定義 -----------------------


//inline float Weight(float dist, float re)
//{
//    return ((re / dist) - 1.0); //重み関数
//}
// ▼ ★ここから本体 --------------------------------------

/// Poly6 Kernel の実装\
/// W_poly6(r, h) = DensityCoef * (h^2 - r^2)^3
/// -> DensityCoef = ParticleMass * 4 / (PI * Smoothlen^8)
inline float Weight(float r_sq)
{
    const float h_sq = _Smoothlen * _Smoothlen;
    return _DensityCoef * (h_sq - r_sq) * (h_sq - r_sq) * (h_sq - r_sq);
}

/// Becker2007 非圧縮条件の実装:
/// Pressure = B * ((rho / rho_0)^gamma  - 1)
/// 圧力定数Bは正確に計算するべきだが、リアルタイム向きではないので適当な値にする
inline float CalculatePressure(float density)
{
    return _PressureStiffness * max(pow(density / _RestDensity, 7) - 1, 0);
}

/// Spikyカーネルの実装:
/// mass *  Grad(W_Spiky) * (pressure_i- pressure_j)*(density_j / 2)
/// Grad(W_Spiky) = -30 / (pi * h^5) * (h - r)^2
inline float3 CalculateGradPressure(float r, float P_pressure, float N_pressure, float N_density, float3 diff)
{
    const float h = _Smoothlen;
    float avg_pressure = 0.5f * (N_pressure + P_pressure);
    return _GradPressureCoef * avg_pressure / N_density * (h - r) * (h - r) * (diff) / r;
}

/// Viscosityカーネルの実装:
/// mass * (u_j - u_i) / density_j * Laplacian(W_viscosity)
/// Laplacian(W_viscosity) = 20 / (3 * pi * h^5) * (h - r)
inline float3 CalculateLapVelocity(float r, float3 P_velocity, float3 N_velocity, float N_density)
{
    const float h = _Smoothlen;
    float3 vel_diff = (N_velocity - P_velocity);
    return _LapViscosityCoef / N_density * (h - r) * vel_diff;
}

