// �ȗ����̂��߂ɃX���b�h����1�����ɂ��Ă���
//#define THREAD_SIZE_X 1024
#define THREAD_SIZE_X 512

// �� �\�����o�b�t�@�̒�` ---------------------
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

//// ���q�ʒu�Ƒ��x��ێ�����o�b�t�@
//StructuredBuffer<Particle> _ParticlesBufferRead                    : register(t0);
//RWStructuredBuffer<Particle> _ParticlesBufferWrite                 : register(u0);

//// ���q���x��ێ�����o�b�t�@
//StructuredBuffer<ParticleDensity> _ParticlesDensityBufferRead      : register(t1);
//RWStructuredBuffer<ParticleDensity> _ParticlesDensityBufferWrite   : register(u1);

//// ���q���͂�ێ�����o�b�t�@
//StructuredBuffer<ParticlePressure> _ParticlesPressureBufferRead    : register(t2);
//RWStructuredBuffer<ParticlePressure> _ParticlesPressureBufferWrite : register(u2);

//// ���q�����x(��)��ێ�����o�b�t�@
//StructuredBuffer<ParticleForces> _ParticlesForceBufferRead         : register(t3);
//RWStructuredBuffer<ParticleForces> _ParticlesForceBufferWrite      : register(u3);
// �� �\�����o�b�t�@�̒�` ---------------------

// �� �V�F�[�_�萔�̒�` -----------------------
//// �� �V�F�[�_�萔�̒�` -----------------------
//cbuffer CB : register(b0)
//{
//    int _NumParticles; // ���q��
//    float _TimeStep; // ���ԍ��ݕ�(dt)
//    float _Smoothlen; // ���q���a
//    float _PressureStiffness; // Becker�̌W��
//    float _n0; // ���q�����x�̊�l
//    float _lamd; // ��
//    float _DensityCoef; // ���x�Z�o���̌W��
//    float _GradPressureCoef; // ���͎Z�o���̌W��
//    float _LapViscosityCoef; // �S���Z�o���̌W��
//    float _Viscosity; // �S���W��

//    float3 _Range; // �V�~�����[�V�������
//    float _WallStiffness; // �y�i���e�B�@�̉����Ԃ���
//    float3 _Gravity; // �d��
//};
cbuffer CB : register(b0)
{
    int _NumParticles;        // ���q��
    float _TimeStep;          // ���ԍ��ݕ�(dt)
    float _Smoothlen;         // ���q���a
    float _PressureStiffness; // Becker�̌W��
    float _RestDensity;       // �Î~���q�����x
    float _DensityCoef;       // Weight�v�Z���̒萔
    float _GradPressureCoef;  // ���͎Z�o���̌W��
    float _LapViscosityCoef;  // �S���Z�o���̌W��
    float _WallStiffness;     // �y�i���e�B�@�̉����Ԃ���
    float _Viscosity;         // �S���W��
    float _dummy_01;
    float _dummy_02;
    float _dummy_03;
    float3 _Gravity;          // �d��
};

cbuffer CB_1 : register(b1)
{
    float3 _Range_start; // �V�~�����[�V�������
    float _dummy_04;
    float3 _Range_end; // �V�~�����[�V�������
    float _dummy_05;
    float3 _normal_01; // �@��
    float _dummy_06;
    float3 _normal_02; // �@��
    float _dummy_07;
    float3 _normal_03; // �@��
    float _dummy_08;
};
// �� �V�F�[�_�萔�̒�` -----------------------


//inline float Weight(float dist, float re)
//{
//    return ((re / dist) - 1.0); //�d�݊֐�
//}
// �� ����������{�� --------------------------------------

/// Poly6 Kernel �̎���\
/// W_poly6(r, h) = DensityCoef * (h^2 - r^2)^3
/// -> DensityCoef = ParticleMass * 4 / (PI * Smoothlen^8)
inline float Weight(float r_sq)
{
    const float h_sq = _Smoothlen * _Smoothlen;
    return _DensityCoef * (h_sq - r_sq) * (h_sq - r_sq) * (h_sq - r_sq);
}

/// Becker2007 �񈳏k�����̎���:
/// Pressure = B * ((rho / rho_0)^gamma  - 1)
/// ���͒萔B�͐��m�Ɍv�Z����ׂ������A���A���^�C�������ł͂Ȃ��̂œK���Ȓl�ɂ���
inline float CalculatePressure(float density)
{
    return _PressureStiffness * max(pow(density / _RestDensity, 7) - 1, 0);
}

/// Spiky�J�[�l���̎���:
/// mass *  Grad(W_Spiky) * (pressure_i- pressure_j)*(density_j / 2)
/// Grad(W_Spiky) = -30 / (pi * h^5) * (h - r)^2
inline float3 CalculateGradPressure(float r, float P_pressure, float N_pressure, float N_density, float3 diff)
{
    const float h = _Smoothlen;
    float avg_pressure = 0.5f * (N_pressure + P_pressure);
    return _GradPressureCoef * avg_pressure / N_density * (h - r) * (h - r) * (diff) / r;
}

/// Viscosity�J�[�l���̎���:
/// mass * (u_j - u_i) / density_j * Laplacian(W_viscosity)
/// Laplacian(W_viscosity) = 20 / (3 * pi * h^5) * (h - r)
inline float3 CalculateLapVelocity(float r, float3 P_velocity, float3 N_velocity, float N_density)
{
    const float h = _Smoothlen;
    float3 vel_diff = (N_velocity - P_velocity);
    return _LapViscosityCoef / N_density * (h - r) * vel_diff;
}

