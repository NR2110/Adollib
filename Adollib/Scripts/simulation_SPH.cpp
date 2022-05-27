
#include "Simulation_SPH.h"
#include <unordered_map>

#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Renderer/renderer_base.h"

#include "../Adollib/Scripts/Imgui/work_meter.h"

#include "../Adollib/Scripts/Renderer/Shader/compute_shader.h"

#include "../Adollib/Scripts/Main/systems.h"

using namespace Adollib;
using namespace Compute_S;

#define USE_22 0
#define USE_COMPUTE 1
#define USE_COMPUTE_22 0

//#define USE_3D

#if 0
/*----------------------------
  Constants
----------------------------*/

static const float SPH_RESTDENSITY = 600.0;
static const float SPH_INTSTIFF = 3.0;
static const float SPH_PMASS = 0.00020543;
static const float SPH_SIMSCALE = 0.004;
static const float H = 0.01;
static const float DT = 0.004;
static const float SPH_VISC = 0.2;
static const float SPH_LIMIT = 200.0;
static const float SPH_RADIUS = 0.004;
static const float SPH_EPSILON = 0.00001;
static const float SPH_EXTSTIFF = 10000.0;
static const float SPH_EXTDAMP = 256.0;
static const float SPH_PDIST = pow(SPH_PMASS / SPH_RESTDENSITY,
	1.0 / 3.0);
static const Vector3   MIN = Vector3(0.0, 0.0, -10.0);
static const Vector3   MAX = Vector3(20.0, 50.0, 10.0);
static const Vector3   INIT_MIN = Vector3(0.0, 0.0, -10.0);
static const Vector3   INIT_MAX = Vector3(10.0, 20.0, 10.0);
static const float Poly6Kern = 315.0f / (64.0f * PI * pow(H, 9));
static const float SpikyKern = -45.0f / (PI * pow(H, 6));
static const float LapKern = 45.0f / (PI * pow(H, 6));

#define FOR_EACH_PARTICLE( p_p, p_ps ) \
  for( Particles::iterator (p_p) = (p_ps)->begin(); (p_p) != (p_ps)->end(); (p_p)++ )


void Simulation_SPH::awake() {

	float d = SPH_PDIST / SPH_SIMSCALE * 0.95;
	for (float x = INIT_MIN.x + d; x <= INIT_MAX.x - d; x += d)
		for (float y = INIT_MIN.y + d; y <= INIT_MAX.y - d; y += d)
			for (float z = INIT_MIN.z + d; z <= INIT_MAX.z - d; z += d)
			{
				Particle p;
				p.pos.x = x; p.pos.y = y; p.pos.z = z;
				p.velocity.x = 0.0; p.velocity.y = 0.0; p.velocity.z = 0.0;
				p.force.x = 0.0; p.force.y = 0.0; p.force.z = 0.0;
				p.density = 0.0;
				p.pressure = 0.0;

				p.gameobject = Gameobject_manager::createSphere("particle");
				p.gameobject->transform->local_scale = Vector3(1) * 0.5f;
				particles.emplace_back(p);
			}

}

void Simulation_SPH::update() {

	simulation(&particles);

	for (auto& particle : particles) {
		particle.gameobject->transform->local_pos = particle.pos;
	}

}


void Simulation_SPH::simulation(std::vector<Particle>* p_ps)
{
	calculate_density_and_pressure(p_ps);
	calculate_force(p_ps);
	calculate_position(p_ps);
}

// 密度と圧力の計算
void Simulation_SPH::calculate_density_and_pressure(std::vector<Particle>* ps)
{
	float r2, c, sum, H2;
	Vector3 dr;

	H2 = H * H;

	for (std::vector<Particle>::iterator(p) = (ps)->begin(); (p) != (ps)->end(); (p)++)
	{
		sum = 0.0;
		for (std::vector<Particle>::iterator(pj) = (ps)->begin(); (pj) != (ps)->end(); (pj)++)
		{
			if (p == pj) continue;
			dr = (p->pos - pj->pos) * SPH_SIMSCALE;
			r2 = (dr).norm();
			if (H2 > r2)
			{
				c = H2 - r2;
				sum += c * c * c;
			}
			cb._DensityCoef = particleMass * 4.f / (PI * pow(smoothlen, 8));      // 密度算出時の係数
			cb._GradPressureCoef = particleMass * -30.0f / (PI * pow(smoothlen, 5));   // 圧力算出時の係数
			cb._LapViscosityCoef = particleMass * 20.f / (3 * PI * pow(smoothlen, 5)); // 粘性算出時の係数

		}
		p->density = sum * SPH_PMASS * Poly6Kern;
		p->pressure = (p->density - SPH_RESTDENSITY) * SPH_INTSTIFF;
		p->density = 1.0 / p->density;  // take inverse for later calculation
	}
}

// 力の計算
void Simulation_SPH::calculate_force(std::vector<Particle>* ps)
{
	float pterm, vterm, r, c;
	Vector3 dr(0), force(0), fcurr(0);

	for (std::vector<Particle>::iterator(p) = (ps)->begin(); (p) != (ps)->end(); (p)++)
	{
		force = Vector3(0);
		for (std::vector<Particle>::iterator(pj) = (ps)->begin(); (pj) != (ps)->end(); (pj)++)
		{
			if (p == pj) continue;
			dr = (p->pos - pj->pos) * SPH_SIMSCALE;
			r = (dr).norm_sqr();
			if (H > r)
			{
				c = H - r;
				pterm = -0.5 * c * SpikyKern * (p->pressure + pj->pressure) / r;
				vterm = LapKern * SPH_VISC;
				fcurr = pterm * dr + vterm * (pj->velocity - p->velocity);
				fcurr *= c * p->density * pj->density;
				force += fcurr;
			}
		}
		p->force = force;
	}
}

// 境界条件と粒子位置の更新
void Simulation_SPH::calculate_position(std::vector<Particle>* ps)
{
	Vector3 accel(0), g(0), norm(0);
	float speed, diff, adj;

	g = Vector3(0, -9.8, 0);
	for (std::vector<Particle>::iterator(p) = (ps)->begin(); (p) != (ps)->end(); (p)++)
	{
		accel = p->force * SPH_PMASS;

		speed = (accel).norm();
		if (speed > SPH_LIMIT * SPH_LIMIT) {
			accel *= SPH_LIMIT / sqrt(speed);
		}

		// Z-axis walls
		diff = 2.0 * SPH_RADIUS - (p->pos[2] - MIN[2]) * SPH_SIMSCALE;
		if (diff > FLT_EPSILON)
		{
			norm = Vector3(0, 0, 1);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * vector3_dot(norm, p->velocity);
			accel += adj * norm;
		}
		diff = 2.0 * SPH_RADIUS - (MAX[2] - p->pos[2]) * SPH_SIMSCALE;
		if (diff > FLT_EPSILON)
		{
			norm = Vector3(0, 0, -1);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * vector3_dot(norm, p->velocity);
			accel += adj * norm;
		}

		// X-axis walls
		diff = 2.0 * SPH_RADIUS - (p->pos[0] - MIN[0]) * SPH_SIMSCALE;
		if (diff > FLT_EPSILON)
		{
			norm = Vector3(1, 0, 0);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * vector3_dot(norm, p->velocity);
			accel += adj * norm;
		}
		diff = 2.0 * SPH_RADIUS - (MAX[0] - p->pos[0]) * SPH_SIMSCALE;
		if (diff > FLT_EPSILON)
		{
			norm = Vector3(-1, 0, 0);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * vector3_dot(norm, p->velocity);
			accel += adj * norm;
		}

		// Y-axis walls
		diff = 2.0 * SPH_RADIUS - (p->pos[1] - MIN[1]) * SPH_SIMSCALE;
		if (diff > FLT_EPSILON)
		{
			norm = Vector3(0, 1, 0);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * vector3_dot(norm, p->velocity);
			accel += adj * norm;
		}
		diff = 2.0 * SPH_RADIUS - (MAX[1] - p->pos[1]) * SPH_SIMSCALE;
		if (diff > FLT_EPSILON)
		{
			norm = Vector3(0, -1, 0);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * vector3_dot(norm, p->velocity);
			accel += adj * norm;
		}

		accel += g;
		p->velocity += accel * DT;
		p->pos += p->velocity * DT / SPH_SIMSCALE;

		// stick on x-y plane
		p->pos[2] = 0.0;

	}
}

#endif

#if 0

#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>

/*----------------------------
  Declaration of vec3
----------------------------*/

struct vec3 {
	float x, y, z;
	vec3() {
		x = 0; y = 0; z = 0;
	}
	vec3(const float _x, const float _y, const float _z) {
		x = _x; y = _y; z = _z;
	}
};

inline void operator+= (vec3& v, const vec3 a) {
	v.x += a.x; v.y += a.y; v.z += a.z;
}

inline void operator*= (vec3& v, const float a) {
	v.x *= a; v.y *= a; v.z *= a;
}

inline bool operator== (const vec3 a, const vec3 b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline vec3 operator- (const vec3 a)
{
	return vec3(-a.x, -a.y, -a.z);
}

inline vec3 operator+ (const vec3 a, const vec3 b)
{
	vec3 v;
	v.x = a.x + b.x;
	v.y = a.y + b.y;
	v.z = a.z + b.z;
	return v;
}

inline vec3 operator- (const vec3 a, const vec3 b)
{
	vec3 v;
	v.x = a.x - b.x;
	v.y = a.y - b.y;
	v.z = a.z - b.z;
	return v;
}

inline vec3 operator* (const float a, const vec3 b)
{
	vec3 v;
	v.x = a * b.x;
	v.y = a * b.y;
	v.z = a * b.z;
	return v;
}

inline vec3 operator* (const vec3 a, const float b)
{
	return b * a;
}

inline vec3 operator/ (const vec3 a, const float b)
{
	if (b != 0)
		return a * (1 / b);
	else
	{
		std::cout << "error: division by 0." << std::endl;
		exit(1);
	}
}

inline float dot(const vec3 a, const vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float mag2(const vec3 a)
{
	return a.x * a.x + a.y * a.y + a.z * a.z;
}

inline float mag(const vec3 a)
{
	return sqrt(mag2(a));
}


/*----------------------------
  Constants
----------------------------*/

static const float SPH_RESTDENSITY = 600.0; //定常密度 密度がこの値になるように動く
static const float SPH_INTSTIFF = 3.0; //圧力係数 大きくすることで、非圧縮性を近似する
static const float SPH_PMASS = 0.00020543; //質量
static const float SPH_SIMSCALE = 0.004; //シミューレションのスケール
static const float H = 0.01; //影響半径
//static const float PI = 3.141592653589793;
static const float DT = 0.004;
static const float SPH_VISC = 0.2; //粘性係数
static const float SPH_LIMIT = 200.0; //境界条件における、速度の上限
static const float SPH_RADIUS = 0.004; //
static const float SPH_EPSILON = 0.00001; //
static const float SPH_EXTSTIFF = 10000.0; //境界条件の係数
static const float SPH_EXTDAMP = 256.0; //境界条件の係数
static const float SPH_PDIST = pow(SPH_PMASS / SPH_RESTDENSITY, 1.0 / 3.0);
static const vec3   MIN = vec3(0.0, 0.0, -10.0);
static const vec3   MAX = vec3(20.0, 50.0, 10.0);
static const vec3   INIT_MIN = vec3(0.0, 10.0, -10.0);
static const vec3   INIT_MAX = vec3(10.0, 30.0, 10.0);
static const float Poly6Kern = 315.0 / (64.0 * PI * pow(H, 9));
static const float SpikyKern = -45.0 / (PI * pow(H, 6));
static const float LapKern = 45.0 / (PI * pow(H, 6));


/*----------------------------
  Declaration of Particle
----------------------------*/

struct Particle
{
	vec3 pos, vel, f;
	float rho, prs;
	Gameobject* gameobject = nullptr; //描画用
};
typedef std::vector<Particle> Particles;

Particles* new_particles()
{
	Particles* p_ps;
	p_ps = new Particles();

	float d = SPH_PDIST / SPH_SIMSCALE * 0.95;
	for (float x = INIT_MIN.x + d; x <= INIT_MAX.x - d; x += d)
		for (float y = INIT_MIN.y + d; y <= INIT_MAX.y - d; y += d)
			for (float z = INIT_MIN.z + d; z <= INIT_MAX.z - d; z += d)
			{
				Particle p;
				p.pos.x = x; p.pos.y = y; p.pos.z = z;
				p.vel.x = 0.0; p.vel.y = 0.0; p.vel.z = 0.0;
				p.f.x = 0.0; p.f.y = 0.0; p.f.z = 0.0;
				p.rho = 0.0;
				p.prs = 0.0;

				p.gameobject = Gameobject_manager::createSphere("particle");
				p.gameobject->transform->local_scale = Vector3(1) * 0.3f;
				p_ps->emplace_back(p);
			}
	return p_ps;
}

void delete_particles(Particles* p_ps)
{
	delete p_ps;
}

int particles_size(Particles* p_ps)
{
	return p_ps->size();
}



/*----------------------------
  Declaration of NeighborMap
----------------------------*/

typedef long NeighborMapIdx;
typedef std::list< Particle* > ParticlePtrs;
typedef std::map< NeighborMapIdx, ParticlePtrs > NeighborMap;

NeighborMap* new_neighbor_map(Particles* p_ps);
void           _insert_neighbor_map(Particle*, NeighborMap*);
void           delete_neighbor_map(NeighborMap*);
ParticlePtrs   neighbor(NeighborMap*, vec3);
NeighborMapIdx neighbor_map_idx(vec3);

#define FOR_EACH_PARTICLE_PTR( p_ptr, p_ptrs ) \
  for( ParticlePtrs::iterator p_ptr = (p_ptrs)->begin(); (p_ptr) != (p_ptrs)->end(); (p_ptr)++ )

NeighborMap* new_neighbor_map(Particles* p_ps)
{
	NeighborMap* p_nbr_map;
	p_nbr_map = new NeighborMap();
	for (Particles::iterator(p_p) = (p_ps)->begin(); (p_p) != (p_ps)->end(); (p_p)++)
		_insert_neighbor_map(&*p_p, p_nbr_map);
	return p_nbr_map;
}
void _insert_neighbor_map(Particle* p_p, NeighborMap* p_nbr_map)
{
	NeighborMapIdx ix = neighbor_map_idx(p_p->pos);
	NeighborMap::iterator iter = p_nbr_map->find(ix);
	if (iter != p_nbr_map->end())
	{
		iter->second.push_back(p_p);
	}
	else
	{
		ParticlePtrs ptrs;
		ptrs.push_back(p_p);
		p_nbr_map->insert(NeighborMap::value_type(ix, ptrs));
	}
}
void delete_neighbor_map(NeighborMap* p_nbr_map)
{
	delete p_nbr_map;
}
ParticlePtrs neighbor(NeighborMap* p_nbr_map, vec3 r)
{
	ParticlePtrs ptrs;
	float d = H / SPH_SIMSCALE;
	for (int x = -1; x < 2; x++)
		for (int y = -1; y < 2; y++)
			for (int z = -1; z < 2; z++)
			{
				vec3 v(r.x + (float)x * d, r.y + (float)y * d, r.z + (float)z * d);
				if (MIN.x <= v.x && v.x <= MAX.x &&
					MIN.y <= v.y && v.y <= MAX.y &&
					MIN.z <= v.z && v.z <= MAX.z)
				{
					NeighborMapIdx ix = neighbor_map_idx(v);
					NeighborMap::iterator x = p_nbr_map->find(ix);
					if (x != p_nbr_map->end())
					{
						FOR_EACH_PARTICLE_PTR(p_ptr, &(x->second))
						{
							ptrs.push_back(*p_ptr);
						}
					}
				}
			}
	return ptrs;
}

// 空間分割法
NeighborMapIdx neighbor_map_idx(vec3 r)
{
	int x, y, z;
	int mx, my;
	float d;
	d = H / SPH_SIMSCALE;
	x = floor((r.x - MIN.x) / d);
	y = floor((r.y - MIN.y) / d);
	z = floor((r.z - MIN.z) / d);
	mx = floor((MAX.x - MIN.x) / d);
	my = floor((MAX.y - MIN.y) / d);
	return x + y * mx + z * mx * my;
}


/*----------------------------
  Functions for simulation
----------------------------*/

void simulation(Particles*);
void calc_amount(Particles*, NeighborMap*);
void calc_force(Particles*, NeighborMap*);
void advance(Particles*, NeighborMap*);

void simulation(Particles* p_ps)
{
	NeighborMap* p_nbr_map;
	p_nbr_map = new_neighbor_map(p_ps);
	calc_amount(p_ps, p_nbr_map);
	calc_force(p_ps, p_nbr_map);
	advance(p_ps, p_nbr_map);
	delete_neighbor_map(p_nbr_map);
}

void calc_amount(Particles* p_ps, NeighborMap* p_nbr_map)
{
	float H2, sum, r2, c;
	vec3 dr;
	ParticlePtrs ptrs;
	Particle* p_pj;

	H2 = H * H;

	// すべての
	for (Particles::iterator(p_p) = (p_ps)->begin(); (p_p) != (p_ps)->end(); (p_p)++)
	{
		sum = 0.0;
		ptrs = neighbor(p_nbr_map, p_p->pos);
		FOR_EACH_PARTICLE_PTR(p_ptr, &ptrs)
		{
			p_pj = *p_ptr;
			dr = (p_p->pos - p_pj->pos) * SPH_SIMSCALE;
			r2 = mag2(dr);
			if (H2 > r2)
			{
				c = H2 - r2;
				sum += c * c * c;
			}
		}
		p_p->rho = sum * SPH_PMASS * Poly6Kern;
		p_p->prs = (p_p->rho - SPH_RESTDENSITY) * SPH_INTSTIFF;
		p_p->rho = 1.0 / p_p->rho;
	}
}

void calc_force(Particles* p_ps, NeighborMap* p_nbr_map)
{
	float pterm, vterm, r, c;
	vec3 dr, force, fcurr;
	ParticlePtrs ptrs;
	Particle* p_pj;

	for (Particles::iterator(p_p) = (p_ps)->begin(); (p_p) != (p_ps)->end(); (p_p)++)
	{
		force = vec3(0.0, 0.0, 0.0);
		ptrs = neighbor(p_nbr_map, p_p->pos);
		FOR_EACH_PARTICLE_PTR(p_ptr, &ptrs)
		{
			p_pj = *p_ptr;
			if (p_p->pos == p_pj->pos) continue;
			dr = (p_p->pos - p_pj->pos) * SPH_SIMSCALE;
			r = mag(dr);
			if (H > r)
			{
				c = H - r;
				pterm = -0.5 * c * SpikyKern * (p_p->prs + p_pj->prs) / r;
				vterm = LapKern * SPH_VISC;
				fcurr = pterm * dr + vterm * (p_pj->vel - p_p->vel);
				fcurr *= c * p_p->rho * p_pj->rho;
				force += fcurr;
			}
		}
		p_p->f = force;
	}
}

void advance(Particles* p_ps, NeighborMap* p_nbr_map)
{
	vec3 accel, g, norm;
	float speed, diff, adj;

	g = vec3(0.0, -9.8, 0.0);
	for (Particles::iterator(p_p) = (p_ps)->begin(); (p_p) != (p_ps)->end(); (p_p)++)
	{
		accel = p_p->f * SPH_PMASS;

		speed = mag2(accel);
		if (speed > SPH_LIMIT * SPH_LIMIT) {
			accel *= SPH_LIMIT / sqrt(speed);
		}

		// Z-axis walls
		diff = 2.0 * SPH_RADIUS - (p_p->pos.z - MIN.z) * SPH_SIMSCALE;
		if (diff > SPH_EPSILON)
		{
			norm = vec3(0.0, 0.0, 1.0);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * dot(norm, p_p->vel);
			accel += adj * norm;
		}
		diff = 2.0 * SPH_RADIUS - (MAX.z - p_p->pos.z) * SPH_SIMSCALE;
		if (diff > SPH_EPSILON)
		{
			norm = vec3(0.0, 0.0, -1.0);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * dot(norm, p_p->vel);
			accel += adj * norm;
		}

		// X-axis walls
		diff = 2.0 * SPH_RADIUS - (p_p->pos.x - MIN.x) * SPH_SIMSCALE;
		if (diff > SPH_EPSILON)
		{
			norm = vec3(1.0, 0.0, 0.0);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * dot(norm, p_p->vel);
			accel += adj * norm;
		}
		diff = 2.0 * SPH_RADIUS - (MAX.x - p_p->pos.x) * SPH_SIMSCALE;
		if (diff > SPH_EPSILON)
		{
			norm = vec3(-1.0, 0.0, 0.0);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * dot(norm, p_p->vel);
			accel += adj * norm;
		}

		// Y-axis walls
		diff = 2.0 * SPH_RADIUS - (p_p->pos.y - MIN.y) * SPH_SIMSCALE;
		if (diff > SPH_EPSILON)
		{
			norm = vec3(0.0, 1.0, 0.0);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * dot(norm, p_p->vel);
			accel += adj * norm;
		}
		diff = 2.0 * SPH_RADIUS - (MAX.y - p_p->pos.y) * SPH_SIMSCALE;
		if (diff > SPH_EPSILON)
		{
			norm = vec3(0.0, -1.0, 0.0);
			adj = SPH_EXTSTIFF * diff - SPH_EXTDAMP * dot(norm, p_p->vel);
			accel += adj * norm;
		}

		accel += g;
		p_p->vel += accel * DT;
		p_p->pos += p_p->vel * DT / SPH_SIMSCALE;
	}
}


/*----------------------------
  Functions for output
----------------------------*/

void output_particles(Particles* p_ps, int cnt)
{
	for (auto& particle : *p_ps) {
		particle.gameobject->transform->local_pos = Vector3(particle.pos.x, particle.pos.y, particle.pos.z);
	}

}


/*----------------------------
  main function
----------------------------*/
static Particles* p_ps;

void Simulation_SPH::awake() {
	p_ps = new_particles();
}

void Simulation_SPH::update()
{
	for (int i = 0; i < 1; i++)
	{
		output_particles(p_ps, i);
		simulation(p_ps);
	}
	//delete_particles(p_ps);
}


#endif

#if USE_22
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define IN_FILE "../mk_particle/dambreak.prof"
#define PARTICLE_DISTANCE    0.025 //初期粒子間距離
#define EPS             (0.01 * PARTICLE_DISTANCE)
#define DIM 2				//次元数

#define PCL_DST PARTICLE_DISTANCE //平均粒子間距離
#define MIN_X  (0 - PCL_DST*5)	//解析領域のx方向の最小値
#define MIN_Y  (0 - PCL_DST*5)	//解析領域のy方向の最小値
#define MIN_Z  (0 - PCL_DST*5)	//解析領域のz方向の最小値
#define MAX_X  (0 + PCL_DST*100)	//解析領域のx方向の最大値
#define MAX_Y  (0 + PCL_DST*80)	//解析領域のy方向の最大値
#define MAX_Z  (0 + PCL_DST*30)	//解析領域のz方向の最大値

#define GST -1			//計算対象外粒子の種類番号
#define FLD 0				//流体粒子の種類番号
#define WLL  1			//壁粒子の種類番号
#define NUM_TYP  2		//粒子の種類数
#define DNS_FLD 1000		//流体粒子の密度
#define DNS_WLL 1000		//壁粒子の密度
#define DT 0.0005			//時間刻み幅
#define SND 22.0			//音速
#define KNM_VSC (1.0E-6)	//動粘性係数
#define CRT_NUM 0.1		//クーラン条件数
#define COL_RAT 0.2		//接近した粒子の反発率
#define DST_LMT_RAT 0.5	//これ以上の粒子間の接近を許さない距離の係数
#define G_X 0.0			//重力加速度のx成分
#define G_Y -9.8			//重力加速度のy成分
#define G_Z 0.0			//重力加速度のz成分
#define WEI(dist, re) ((re/dist) - 1.0)	//重み関数

char filename[256];
int iLP, iF;
float TIM;
int nP;
float* Acc, * Pos, * Vel, * Prs, * pav;
int* Typ;
float r, r2;
float DB, DB2, DBinv;
int nBx, nBy, nBz, nBxy, nBxyz;
int* bfst, * blst, * nxt;
float n0, lmd, A1, A2, A3, rlim, rlim2, COL;
float Dns[NUM_TYP], invDns[NUM_TYP];

static std::vector<Gameobject*> particle;

void RdDat();
void AlcBkt();
void SetPara();
void ClcEMPS();

void Simulation_SPH::awake() {

	RdDat(); //粒子の配置
	AlcBkt(); //定数の入力
	SetPara(); //計算できる定数の入力(n0など)

	particle.resize(nP);
	for (int i = 0; i < nP; i++) {
		//if (Pos[i * 3 + 2] < EPS)continue;
		particle[i] = Gameobject_manager::createSphere("particle");
		particle[i]->transform->local_pos = Vector3(Pos[i * 3], Pos[i * 3 + 1], Pos[i * 3 + 2]);
		particle[i]->transform->local_scale = Vector3((2.1 * PARTICLE_DISTANCE) * 0.4f);
		particle[i]->is_hierarchy = false;

		if (Typ[i] == FLD)particle[i]->renderer->color = Vector4(0, 0, 1, 1);
		if (Typ[i] == WLL)particle[i]->renderer->color = Vector4(0, 1, 0, 1);
		if (Typ[i] == GST)particle[i]->renderer->color = Vector4(1, 0, 0, 1);
	}

}

void Simulation_SPH::update() {

	Work_meter::start("simulation_SPH");
	ClcEMPS(); //mein処理
	for (int i = 0; i < nP; i++) {
		//particle.gameobject->transform->local_pos = particle.pos;
		if (Typ[i] != FLD)continue;
		particle[i]->transform->local_pos = Vector3(Pos[i * 3], Pos[i * 3 + 1], Pos[i * 3 + 2]);

	}
	Work_meter::stop("simulation_SPH");
}


void ChkPcl(int i) {
	if (Pos[i * 3] > MAX_X || Pos[i * 3]<MIN_X ||
		Pos[i * 3 + 1]>MAX_Y || Pos[i * 3 + 1]<MIN_Y ||
		Pos[i * 3 + 2]>MAX_Z || Pos[i * 3 + 2] < MIN_Z)
	{
		Typ[i] = GST;
		Prs[i] = Vel[i * 3] = Vel[i * 3 + 1] = Vel[i * 3 + 2] = 0.0;
	}
}

void RdDat(void) {
	nP = 17000;
	Acc = (float*)malloc(sizeof(float) * nP * 3);	//粒子の加速度
	Pos = (float*)malloc(sizeof(float) * nP * 3);	//粒子の座標
	Vel = (float*)malloc(sizeof(float) * nP * 3);	//粒子の速度
	Prs = (float*)malloc(sizeof(float) * nP);		//粒子の圧力
	pav = (float*)malloc(sizeof(float) * nP);		//時間平均された粒子の圧力
	Typ = (int*)malloc(sizeof(int) * nP);			//粒子の種類

#if DIM == 3
	int iX, iY, iZ;
	int nX, nY, nZ;
	float x, y, z;
	int i = 0;
	int flagOfParticleGeneration;

	nX = (int)(1.0 / PARTICLE_DISTANCE) + 5;
	nY = (int)(0.6 / PARTICLE_DISTANCE) + 5;
	nZ = (int)(0.3 / PARTICLE_DISTANCE) + 5;
	for (iX = -4; iX < nX; iX++) {
		for (iY = -4; iY < nY; iY++) {
			for (iZ = -4; iZ < nZ; iZ++) {
				x = PARTICLE_DISTANCE * iX;
				y = PARTICLE_DISTANCE * iY;
				z = PARTICLE_DISTANCE * iZ;
				flagOfParticleGeneration = 0;

				/* dummy wall region */
				if ((((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 4.0 * PARTICLE_DISTANCE + EPS))) {
					Typ[i] = WLL;
					flagOfParticleGeneration = 1;
				}

				/* wall region */
				if ((((x > -2.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 2.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 2.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 2.0 * PARTICLE_DISTANCE + EPS))) {
					Typ[i] = WLL;
					flagOfParticleGeneration = 1;
				}

				/* wall region */
				if ((((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.6 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 4.0 * PARTICLE_DISTANCE + EPS))) {
					Typ[i] = WLL;
					flagOfParticleGeneration = 1;
				}

				/* empty region */
				if ((((x > 0.0 + EPS) && (x <= 1.00 + EPS)) && (y > 0.0 + EPS)) && ((z > 0.0 + EPS) && (z <= 0.3 + EPS))) {
					flagOfParticleGeneration = 0;
				}

				/* fluid region */
				if ((((x > 0.0 + EPS) && (x <= 0.25 + EPS)) && ((y > 0.0 + EPS) && (y < 0.5 + EPS))) && ((z > 0.0 + EPS) && (z <= 0.3 + EPS))) {
					Typ[i] = FLD;
					flagOfParticleGeneration = 1;
				}

				if (flagOfParticleGeneration == 1) {
					Pos[i * 3] = x;
					Pos[i * 3 + 1] = y;
					Pos[i * 3 + 2] = z;
					i++;
				}
			}
		}
	}
	nP = i;
	for (i = 0; i < nP * 3; i++) { Vel[i] = 0.0; }
#elif  DIM == 2
	  // 各粒子の初期座標など
	int iX, iY;
	int nX, nY;
	float x, y, z;
	int i = 0;
	int flagOfParticleGeneration;

	nX = (int)(2.0 / PARTICLE_DISTANCE) + 5;
	nY = (int)(1.0 / PARTICLE_DISTANCE) + 5;
	for (iX = -4; iX < nX; iX++) {
		for (iY = -4; iY < nY; iY++) {
			x = PARTICLE_DISTANCE * (float)(iX);
			y = PARTICLE_DISTANCE * (float)(iY);
			z = 0.0;
			flagOfParticleGeneration = 0;

			/* dummy wall region */
			if (((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 2.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (y <= 1 + 4.0 * PARTICLE_DISTANCE + EPS))) {
				Typ[i] = WLL;
				flagOfParticleGeneration = 1;
			}

			/* wall region */
			if (((x > -2.0 * PARTICLE_DISTANCE + EPS) && (x <= 2.00 + 2.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 1 + 2.0 * PARTICLE_DISTANCE + EPS))) {
				Typ[i] = WLL;
				flagOfParticleGeneration = 1;
			}

			/* wall region */
			if (((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 2.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.6 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 1 + 2.0 * PARTICLE_DISTANCE + EPS))) {
				Typ[i] = WLL;
				flagOfParticleGeneration = 1;
			}


			/* empty region */
			if (((x > 0.0 + EPS) && (x <= 2.00 + EPS)) && ((y > 0.0 + EPS) && (y <= 1 + EPS))) {
				flagOfParticleGeneration = 0;
			}

			/* fluid region */
			if (((x > 0.0 + EPS) && (x <= 0.5 + EPS)) && ((y > 0.0 + EPS) && (y <= 1 + EPS))) {
				Typ[i] = FLD;
				flagOfParticleGeneration = 1;
			}

			if (flagOfParticleGeneration == 1) {
				Pos[i * 3] = x; Pos[i * 3 + 1] = y; Pos[i * 3 + 2] = z;
				i++;
			}
		}
	}
	nP = i;
	for (i = 0; i < nP * 3; i++) { Vel[i] = 0.0; }
#endif
	for (int i = 0; i < nP; i++) { ChkPcl(i); }
	for (int i = 0; i < nP * 3; i++) { Acc[i] = 0.0; }
}

void AlcBkt(void) {
	r = PCL_DST * 2.1;		//影響半径
	r2 = r * r;
	DB = r * (1.0 + CRT_NUM);	//バケット1辺の長さ
	DB2 = DB * DB;
	DBinv = 1.0 / DB;
	nBx = (int)((MAX_X - MIN_X) * DBinv) + 3;//解析領域内のx方向のバケット数
	nBy = (int)((MAX_Y - MIN_Y) * DBinv) + 3;//解析領域内のy方向のバケット数
	nBz = (int)((MAX_Z - MIN_Z) * DBinv) + 3;//解析領域内のz方向のバケット数
	nBxy = nBx * nBy;
	nBxyz = nBx * nBy * nBz;		//解析領域内のバケット数
	bfst = (int*)malloc(sizeof(int) * nBxyz);	//バケットに格納された先頭の粒子番号
	blst = (int*)malloc(sizeof(int) * nBxyz);	//バケットに格納された最後尾の粒子番号
	nxt = (int*)malloc(sizeof(int) * nP);		//同じバケット内の次の粒子番号
}

void SetPara(void) {
	float tn0 = 0.0;
	float tlmd = 0.0;
#if DEM == 3
	for (int ix = -4; ix < 5; ix++) {
		for (int iy = -4; iy < 5; iy++) {
			for (int iz = -4; iz < 5; iz++) {
				float x = PCL_DST * (float)ix;
				float y = PCL_DST * (float)iy;
				float z = PCL_DST * (float)iz;
				float dist2 = x * x + y * y + z * z;
				//float dist2 = x * x + y * y;
				if (dist2 <= r2) {
					if (dist2 == 0.0)continue;
					float dist = sqrt(dist2);
					tn0 += WEI(dist, r);
					tlmd += dist2 * WEI(dist, r);
				}
			}
		}
	}
#elif DIM == 2
	for (int ix = -4; ix < 5; ix++) {
		for (int iy = -4; iy < 5; iy++) {
			float x = PCL_DST * (float)ix;
			float y = PCL_DST * (float)iy;
			float dist2 = x * x + y * y;
			//float dist2 = x * x + y * y;
			if (dist2 <= r2) {
				if (dist2 == 0.0)continue;
				float dist = sqrt(dist2);
				tn0 += WEI(dist, r);
				tlmd += dist2 * WEI(dist, r);
			}
		}
	}
#endif
	n0 = tn0;			//初期粒子数密度
	lmd = tlmd / tn0;	//ラプラシアンモデルの係数λ
	A1 = 2.0 * KNM_VSC * DIM / n0 / lmd;//粘性項の計算に用いる係数
	A2 = SND * SND / n0;				//圧力の計算に用いる係数
	A3 = -DIM / n0;					//圧力勾配項の計算に用いる係数
	Dns[FLD] = DNS_FLD;
	Dns[WLL] = DNS_WLL;
	invDns[FLD] = 1.0 / DNS_FLD;
	invDns[WLL] = 1.0 / DNS_WLL;
	rlim = PCL_DST * DST_LMT_RAT;//これ以上の粒子間の接近を許さない距離
	rlim2 = rlim * rlim;
	COL = 1.0 + COL_RAT;
	iLP = 0;			//反復数
	iF = 0;			//ファイル番号
	TIM = 0.0;		//時刻
}

void MkBkt(void) {
	for (int i = 0; i < nBxyz; i++) { bfst[i] = -1; }
	for (int i = 0; i < nBxyz; i++) { blst[i] = -1; }
	for (int i = 0; i < nP; i++) { nxt[i] = -1; }
	for (int i = 0; i < nP; i++) {
		if (Typ[i] == GST)continue;
		int ix = (int)((Pos[i * 3] - MIN_X) * DBinv) + 1;
		int iy = (int)((Pos[i * 3 + 1] - MIN_Y) * DBinv) + 1;
		int iz = (int)((Pos[i * 3 + 2] - MIN_Z) * DBinv) + 1;
		int ib = iz * nBxy + iy * nBx + ix;
		int j = blst[ib];
		blst[ib] = i;
		if (j == -1) { bfst[ib] = i; }
		else { nxt[j] = i; }
	}
}

void VscTrm() {
	for (int i = 0; i < nP; i++) {
		if (Typ[i] == FLD) {
			float Acc_x = 0.0;			float Acc_y = 0.0;			float Acc_z = 0.0;
			float pos_ix = Pos[i * 3];	float pos_iy = Pos[i * 3 + 1];	float pos_iz = Pos[i * 3 + 2];
			float vec_ix = Vel[i * 3];	float vec_iy = Vel[i * 3 + 1];	float vec_iz = Vel[i * 3 + 2];
			int ix = (int)((pos_ix - MIN_X) * DBinv) + 1;
			int iy = (int)((pos_iy - MIN_Y) * DBinv) + 1;
			int iz = (int)((pos_iz - MIN_Z) * DBinv) + 1;
			for (int jz = iz - 1; jz <= iz + 1; jz++) {
				for (int jy = iy - 1; jy <= iy + 1; jy++) {
					for (int jx = ix - 1; jx <= ix + 1; jx++) {
						int jb = jz * nBxy + jy * nBx + jx;
						int j = bfst[jb];
						if (j == -1) continue;
						for (;;) {
							float v0 = Pos[j * 3] - pos_ix;
							float v1 = Pos[j * 3 + 1] - pos_iy;
							float v2 = Pos[j * 3 + 2] - pos_iz;
							float dist2 = v0 * v0 + v1 * v1 + v2 * v2;
							if (dist2 < r2) {
								if (j != i && Typ[j] != GST) {
									float dist = sqrt(dist2);
									float w = WEI(dist, r);
									Acc_x += (Vel[j * 3] - vec_ix) * w;
									Acc_y += (Vel[j * 3 + 1] - vec_iy) * w;
									Acc_z += (Vel[j * 3 + 2] - vec_iz) * w;
								}
							}
							j = nxt[j];
							if (j == -1) break;
						}
					}
				}
			}
			Acc[i * 3] = Acc_x * A1 + G_X;
			Acc[i * 3 + 1] = Acc_y * A1 + G_Y;
			Acc[i * 3 + 2] = Acc_z * A1 + G_Z;
		}
	}
}

void UpPcl1() {
	for (int i = 0; i < nP; i++) {
		if (Typ[i] == FLD) {
			Vel[i * 3] += Acc[i * 3] * DT;	Vel[i * 3 + 1] += Acc[i * 3 + 1] * DT;	Vel[i * 3 + 2] += Acc[i * 3 + 2] * DT;
			Pos[i * 3] += Vel[i * 3] * DT;		Pos[i * 3 + 1] += Vel[i * 3 + 1] * DT;		Pos[i * 3 + 2] += Vel[i * 3 + 2] * DT;
			Acc[i * 3] = Acc[i * 3 + 1] = Acc[i * 3 + 2] = 0.0;
			ChkPcl(i);
		}
	}
}

void ChkCol() {
	for (int i = 0; i < nP; i++) {
		if (Typ[i] == FLD) {
			float mi = Dns[Typ[i]];
			float pos_ix = Pos[i * 3];	float pos_iy = Pos[i * 3 + 1];	float pos_iz = Pos[i * 3 + 2];
			float vec_ix = Vel[i * 3];	float vec_iy = Vel[i * 3 + 1];	float vec_iz = Vel[i * 3 + 2];
			float vec_ix2 = Vel[i * 3]; float vec_iy2 = Vel[i * 3 + 1]; float vec_iz2 = Vel[i * 3 + 2];
			int ix = (int)((pos_ix - MIN_X) * DBinv) + 1;
			int iy = (int)((pos_iy - MIN_Y) * DBinv) + 1;
			int iz = (int)((pos_iz - MIN_Z) * DBinv) + 1;
			for (int jz = iz - 1; jz <= iz + 1; jz++) {
				for (int jy = iy - 1; jy <= iy + 1; jy++) {
					for (int jx = ix - 1; jx <= ix + 1; jx++) {
						int jb = jz * nBxy + jy * nBx + jx;
						int j = bfst[jb];
						if (j == -1) continue;
						for (;;) {
							float v0 = Pos[j * 3] - pos_ix;
							float v1 = Pos[j * 3 + 1] - pos_iy;
							float v2 = Pos[j * 3 + 2] - pos_iz;
							float dist2 = v0 * v0 + v1 * v1 + v2 * v2;
							if (dist2 < rlim2) {
								if (j != i && Typ[j] != GST) {
									float fDT = (vec_ix - Vel[j * 3]) * v0 + (vec_iy - Vel[j * 3 + 1]) * v1 + (vec_iz - Vel[j * 3 + 2]) * v2;
									if (fDT > 0.0) {
										float mj = Dns[Typ[j]];
										fDT *= COL * mj / (mi + mj) / dist2;
										vec_ix2 -= v0 * fDT;		vec_iy2 -= v1 * fDT;		vec_iz2 -= v2 * fDT;
									}
								}
							}
							j = nxt[j];
							if (j == -1) break;
						}
					}
				}
			}
			Acc[i * 3] = vec_ix2;	Acc[i * 3 + 1] = vec_iy2;	Acc[i * 3 + 2] = vec_iz2;
		}
	}
	for (int i = 0; i < nP; i++) {
		Vel[i * 3] = Acc[i * 3];	Vel[i * 3 + 1] = Acc[i * 3 + 1];	Vel[i * 3 + 2] = Acc[i * 3 + 2];
	}
}

void MkPrs() {
	for (int i = 0; i < nP; i++) {
		if (Typ[i] != GST) {
			float pos_ix = Pos[i * 3];	float pos_iy = Pos[i * 3 + 1];	float pos_iz = Pos[i * 3 + 2];
			float ni = 0.0;
			int ix = (int)((pos_ix - MIN_X) * DBinv) + 1;
			int iy = (int)((pos_iy - MIN_Y) * DBinv) + 1;
			int iz = (int)((pos_iz - MIN_Z) * DBinv) + 1;
			for (int jz = iz - 1; jz <= iz + 1; jz++) {
				for (int jy = iy - 1; jy <= iy + 1; jy++) {
					for (int jx = ix - 1; jx <= ix + 1; jx++) {
						int jb = jz * nBxy + jy * nBx + jx;
						int j = bfst[jb];
						if (j == -1) continue;
						for (;;) {
							float v0 = Pos[j * 3] - pos_ix;
							float v1 = Pos[j * 3 + 1] - pos_iy;
							float v2 = Pos[j * 3 + 2] - pos_iz;
							float dist2 = v0 * v0 + v1 * v1 + v2 * v2;
							if (dist2 < r2) {
								if (j != i && Typ[j] != GST) {
									float dist = sqrt(dist2);
									float w = WEI(dist, r);
									ni += w;
								}
							}
							j = nxt[j];
							if (j == -1) break;
						}
					}
				}
			}
			float mi = Dns[Typ[i]];
			float pressure = (ni > n0) * (ni - n0) * A2 * mi;
			Prs[i] = pressure;
		}
	}
}

void PrsGrdTrm() {
	for (int i = 0; i < nP; i++) {
		if (Typ[i] == FLD) {
			float Acc_x = 0.0;			float Acc_y = 0.0;			float Acc_z = 0.0;
			float pos_ix = Pos[i * 3];	float pos_iy = Pos[i * 3 + 1];	float pos_iz = Pos[i * 3 + 2];
			float pre_min = Prs[i];
			int ix = (int)((pos_ix - MIN_X) * DBinv) + 1;
			int iy = (int)((pos_iy - MIN_Y) * DBinv) + 1;
			int iz = (int)((pos_iz - MIN_Z) * DBinv) + 1;
			// 周囲の粒子の最低圧力を保存
			for (int jz = iz - 1; jz <= iz + 1; jz++) {
				for (int jy = iy - 1; jy <= iy + 1; jy++) {
					for (int jx = ix - 1; jx <= ix + 1; jx++) {
						int jb = jz * nBxy + jy * nBx + jx;
						int j = bfst[jb];
						if (j == -1) continue;
						for (;;) {
							float v0 = Pos[j * 3] - pos_ix;
							float v1 = Pos[j * 3 + 1] - pos_iy;
							float v2 = Pos[j * 3 + 2] - pos_iz;
							float dist2 = v0 * v0 + v1 * v1 + v2 * v2;
							if (dist2 < r2) {
								if (j != i && Typ[j] != GST) {
									if (pre_min > Prs[j])pre_min = Prs[j];
								}
							}
							j = nxt[j];
							if (j == -1) break;
						}
					}
				}
			}
			for (int jz = iz - 1; jz <= iz + 1; jz++) {
				for (int jy = iy - 1; jy <= iy + 1; jy++) {
					for (int jx = ix - 1; jx <= ix + 1; jx++) {
						int jb = jz * nBxy + jy * nBx + jx;
						int j = bfst[jb];
						if (j == -1) continue;
						for (;;) {
							float v0 = Pos[j * 3] - pos_ix;
							float v1 = Pos[j * 3 + 1] - pos_iy;
							float v2 = Pos[j * 3 + 2] - pos_iz;
							float dist2 = v0 * v0 + v1 * v1 + v2 * v2;
							if (dist2 < r2) {
								if (j != i && Typ[j] != GST) {
									float dist = sqrt(dist2);
									float w = WEI(dist, r);
									w *= (Prs[j] - pre_min) / dist2;
									Acc_x += v0 * w;	Acc_y += v1 * w;	Acc_z += v2 * w;
								}
							}
							j = nxt[j];
							if (j == -1) break;
						}
					}
				}
			}
			Acc[i * 3] = Acc_x * invDns[FLD] * A3;
			Acc[i * 3 + 1] = Acc_y * invDns[FLD] * A3;
			Acc[i * 3 + 2] = Acc_z * invDns[FLD] * A3;
		}
	}
}

void UpPcl2(void) {
	for (int i = 0; i < nP; i++) {
		if (Typ[i] == FLD) {
			Vel[i * 3] += Acc[i * 3] * DT;
			Vel[i * 3 + 1] += Acc[i * 3 + 1] * DT;
			Vel[i * 3 + 2] += Acc[i * 3 + 2] * DT;
			Pos[i * 3] += Acc[i * 3] * DT * DT;
			Pos[i * 3 + 1] += Acc[i * 3 + 1] * DT * DT;
			Pos[i * 3 + 2] += Acc[i * 3 + 2] * DT * DT;
			Acc[i * 3] = Acc[i * 3 + 1] = Acc[i * 3 + 2] = 0.0;
			ChkPcl(i);
		}
	}
}

void ClcEMPS(void) {
	{
		//if (iLP % 100 == 0) {
		//	int p_num = 0;
		//	for (int i = 0; i < nP; i++) { if (Typ[i] != GST)p_num++; }
		//	printf("%5d th TIM: %lf / p_num: %d\n", iLP, TIM, p_num);
		//}
		//if (iLP % OPT_FQC == 0) {
		//	if (TIM >= FIN_TIM) { return; }
		//}

		Work_meter::start("save_backet");
		MkBkt(); //バケットの保存
		Work_meter::stop("save_backet");
		Work_meter::start("calc_a*_from_Viscosity_g");
		VscTrm(); //粘着、外力(重力)から仮の加速度a*を計算
		Work_meter::stop("calc_a*_from_Viscosity_g");
		Work_meter::start("update_pos_1");
		UpPcl1(); //a*を用いて仮の速度、座標に更新
		Work_meter::stop("update_pos_1");
		Work_meter::start("collider");
		ChkCol(); //衝突判定 近すぎるものを離す
		Work_meter::stop("collider");
		//:::: 陽解法 :::
		Work_meter::start("presser");
		MkPrs(); //仮の圧力を求める
		Work_meter::stop("presser");
		Work_meter::start("calc_a*_from_presser");
		PrsGrdTrm(); //圧力勾配を求め、圧力による加速度を計算
		Work_meter::stop("calc_a*_from_presser");
		Work_meter::start("update_pos_2");
		UpPcl2(); //圧力による加速度のから速度、座標を更新
		Work_meter::stop("update_pos_2");
		//MkPrs(); //
		for (int i = 0; i < nP; i++) { pav[i] += Prs[i]; }
		iLP++;
		TIM += DT;
	}
}

#elif 0


/*=====================================================================
  mps.c
  (c) Kazuya SHIBATA, Kohei MUROTANI and Seiichi KOSHIZUKA (2014)

   Fluid Simulation Program Based on a Particle Method (the MPS method)
   Last update: May 21, 2014
=======================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define DIM                  2
#define PARTICLE_DISTANCE    0.025 //初期粒子間距離
#define DT                   0.005
#define OUTPUT_INTERVAL      20

/* for three-dimensional simulation */
/*
#define DIM                  3
#define PARTICLE_DISTANCE    0.075
#define DT                   0.003
#define OUTPUT_INTERVAL      2
*/

#define ARRAY_SIZE           5000
#define FINISH_TIME          2.0
#define KINEMATIC_VISCOSITY  (1.0E-6) //動粘性係数
#define FLUID_DENSITY        1000.0
#define G_X  0.0
#define G_Y  -9.8
#define G_Z  0.0
#define RADIUS_FOR_NUMBER_DENSITY  (2.1*PARTICLE_DISTANCE)
#define RADIUS_FOR_GRADIENT        (2.1*PARTICLE_DISTANCE)
#define RADIUS_FOR_LAPLACIAN       (3.1*PARTICLE_DISTANCE)
#define COLLISION_DISTANCE         (0.5*PARTICLE_DISTANCE)
#define THRESHOLD_RATIO_OF_NUMBER_DENSITY  0.97
#define COEFFICIENT_OF_RESTITUTION 0.2
#define COMPRESSIBILITY (0.45E-9)
#define EPS             (0.01 * PARTICLE_DISTANCE)
#define ON              1
#define OFF             0
#define RELAXATION_COEFFICIENT_FOR_PRESSURE 0.2
#define GHOST  -1
#define FLUID   0
#define WALL    2
#define DUMMY_WALL  3
#define GHOST_OR_DUMMY  -1
#define SURFACE_PARTICLE 1
#define INNER_PARTICLE   0
#define DIRICHLET_BOUNDARY_IS_NOT_CONNECTED 0
#define DIRICHLET_BOUNDARY_IS_CONNECTED     1
#define DIRICHLET_BOUNDARY_IS_CHECKED       2

void initializeParticlePositionAndVelocity_for2dim(void);
void initializeParticlePositionAndVelocity_for3dim(void);
void calConstantParameter(void);
void calNZeroAndLambda(void);
float weight(float distance, float re);
void mainLoopOfSimulation(void);
void calGravity(void);
void calViscosity(void);
void moveParticle(void);
void collision(void);
void calPressure(void);
void calNumberDensity(void);
void setBoundaryCondition(void);
void setSourceTerm(void);
void setMatrix(void);
void exceptionalProcessingForBoundaryCondition(void);
void checkBoundaryCondition(void);
void increaseDiagonalTerm(void);
void solveSimultaniousEquationsByGaussEliminationMethod(void);
void removeNegativePressure(void);
void setMinimumPressure(void);
void calPressureGradient(void);
void moveParticleUsingPressureGradient(void);

static float Acceleration[3 * ARRAY_SIZE];
static int    ParticleType[ARRAY_SIZE];
static float Position[3 * ARRAY_SIZE];
static float Velocity[3 * ARRAY_SIZE];
static float Pressure[ARRAY_SIZE];
static float NumberDensity[ARRAY_SIZE];
static int    BoundaryCondition[ARRAY_SIZE];
static float SourceTerm[ARRAY_SIZE];
static int    FlagForCheckingBoundaryCondition[ARRAY_SIZE];
static float CoefficientMatrix[ARRAY_SIZE * ARRAY_SIZE];
static float MinimumPressure[ARRAY_SIZE];
int    FileNumber;
float Time_;
int    NumberOfParticles;
float Re_forNumberDensity, Re2_forNumberDensity;
float Re_forGradient, Re2_forGradient;
float Re_forLaplacian, Re2_forLaplacian;
float N0_forNumberDensity;
float N0_forGradient;
float N0_forLaplacian;
float Lambda;
float collisionDistance, collisionDistance2;
float FluidDensity;

static std::vector<Gameobject*> particle;

void Simulation_SPH::awake() {

	//initializeParticlePositionAndVelocity_for3dim();
	initializeParticlePositionAndVelocity_for2dim();
	calConstantParameter(); //定数の計算

	particle.resize(NumberOfParticles);
	for (int i = 0; i < NumberOfParticles; i++) {
		particle[i] = Gameobject_manager::createSphere("particle");
		particle[i]->transform->local_scale = Vector3(RADIUS_FOR_NUMBER_DENSITY * 0.4f);

		if (ParticleType[i] == FLUID)particle[i]->renderer->color = Vector4(0, 0, 1, 1);
		if (ParticleType[i] == WALL)particle[i]->renderer->color = Vector4(0, 1, 0, 1);
		if (ParticleType[i] == DUMMY_WALL)particle[i]->renderer->color = Vector4(1, 0, 0, 1);
	}
}

void Simulation_SPH::update() {

	mainLoopOfSimulation();


	for (int i = 0; i < NumberOfParticles; i++) {
		//particle.gameobject->transform->local_pos = particle.pos;

		particle[i]->transform->local_pos = Vector3(Position[i * 3], Position[i * 3 + 1], Position[i * 3 + 2]);
	}

}



void initializeParticlePositionAndVelocity_for2dim(void) {
	int iX, iY;
	int nX, nY;
	float x, y, z;
	int i = 0;
	int flagOfParticleGeneration;

	nX = (int)(1.0 / PARTICLE_DISTANCE) + 5;
	nY = (int)(0.6 / PARTICLE_DISTANCE) + 5;
	for (iX = -4; iX < nX; iX++) {
		for (iY = -4; iY < nY; iY++) {
			x = PARTICLE_DISTANCE * (float)(iX);
			y = PARTICLE_DISTANCE * (float)(iY);
			z = 0.0;
			flagOfParticleGeneration = OFF;

			/* dummy wall region */
			if (((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) {
				ParticleType[i] = DUMMY_WALL;
				flagOfParticleGeneration = ON;
			}

			/* wall region */
			if (((x > -2.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 2.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) {
				ParticleType[i] = WALL;
				flagOfParticleGeneration = ON;
			}

			/* wall region */
			if (((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.6 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) {
				ParticleType[i] = WALL;
				flagOfParticleGeneration = ON;
			}

			/* empty region */
			if (((x > 0.0 + EPS) && (x <= 1.00 + EPS)) && (y > 0.0 + EPS)) {
				flagOfParticleGeneration = OFF;
			}

			/* fluid region */
			//if (((x > 0.0 + EPS) && (x <= 0.25 + EPS)) && ((y > 0.0 + EPS) && (y <= 0.50 + EPS))) {
			//	ParticleType[i] = FLUID;
			//	flagOfParticleGeneration = ON;
			//}
			if ((
				(x > 0.03 + EPS) &&
				(x <= 0.05 + EPS)
				) && (
					(y > 0.0 + EPS) && (y <= 0.03 + EPS)
					)) {
				ParticleType[i] = FLUID;
				flagOfParticleGeneration = ON;
			}

			if (flagOfParticleGeneration == ON) {
				Position[i * 3] = x; Position[i * 3 + 1] = y; Position[i * 3 + 2] = z;
				if (ParticleType[i] == FLUID)Position[i * 3] += PARTICLE_DISTANCE * 0.5f;
				i++;
			}
		}
	}
	NumberOfParticles = i;
	for (i = 0; i < NumberOfParticles * 3; i++) { Velocity[i] = 0.0; }
}
void initializeParticlePositionAndVelocity_for3dim(void) {
	int iX, iY, iZ;
	int nX, nY, nZ;
	float x, y, z;
	int i = 0;
	int flagOfParticleGeneration;

	nX = (int)(1.0 / PARTICLE_DISTANCE) + 5;
	nY = (int)(0.6 / PARTICLE_DISTANCE) + 5;
	nZ = (int)(0.3 / PARTICLE_DISTANCE) + 5;
	for (iX = -4; iX < nX; iX++) {
		for (iY = -4; iY < nY; iY++) {
			for (iZ = -4; iZ < nZ; iZ++) {
				x = PARTICLE_DISTANCE * iX;
				y = PARTICLE_DISTANCE * iY;
				z = PARTICLE_DISTANCE * iZ;
				flagOfParticleGeneration = OFF;

				/* dummy wall region */
				if ((((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 4.0 * PARTICLE_DISTANCE + EPS))) {
					ParticleType[i] = DUMMY_WALL;
					flagOfParticleGeneration = ON;
				}

				/* wall region */
				if ((((x > -2.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 2.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 2.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 2.0 * PARTICLE_DISTANCE + EPS))) {
					ParticleType[i] = WALL;
					flagOfParticleGeneration = ON;
				}

				/* wall region */
				if ((((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.6 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 4.0 * PARTICLE_DISTANCE + EPS))) {
					ParticleType[i] = WALL;
					flagOfParticleGeneration = ON;
				}

				/* empty region */
				if ((((x > 0.0 + EPS) && (x <= 1.00 + EPS)) && (y > 0.0 + EPS)) && ((z > 0.0 + EPS) && (z <= 0.3 + EPS))) {
					flagOfParticleGeneration = OFF;
				}

				/* fluid region */
				if ((((x > 0.0 + EPS) && (x <= 0.25 + EPS)) && ((y > 0.0 + EPS) && (y < 0.5 + EPS))) && ((z > 0.0 + EPS) && (z <= 0.3 + EPS))) {
					ParticleType[i] = FLUID;
					flagOfParticleGeneration = ON;
				}

				if (flagOfParticleGeneration == ON) {
					Position[i * 3] = x;
					Position[i * 3 + 1] = y;
					Position[i * 3 + 2] = z;
					i++;
				}
			}
		}
	}
	NumberOfParticles = i;
	for (i = 0; i < NumberOfParticles * 3; i++) { Velocity[i] = 0.0; }
}

void calConstantParameter(void) {

	Re_forNumberDensity = RADIUS_FOR_NUMBER_DENSITY; //影響範囲(粒子数密度用)
	Re_forGradient = RADIUS_FOR_GRADIENT; //影響範囲(グラディエントモデル用)
	Re_forLaplacian = RADIUS_FOR_LAPLACIAN; //影響範囲(ラプシアンモデル用)
	Re2_forNumberDensity = Re_forNumberDensity * Re_forNumberDensity;
	Re2_forGradient = Re_forGradient * Re_forGradient;
	Re2_forLaplacian = Re_forLaplacian * Re_forLaplacian;
	calNZeroAndLambda(); //n0 とλの計算
	FluidDensity = FLUID_DENSITY;
	collisionDistance = COLLISION_DISTANCE;
	collisionDistance2 = collisionDistance * collisionDistance;
	FileNumber = 0;
	Time_ = 0.0;
}


void calNZeroAndLambda(void) {
	int iX, iY, iZ;
	int iZ_start, iZ_end;
	float xj, yj, zj, distance, distance2;
	float xi, yi, zi;

	if (DIM == 2) {
		iZ_start = 0; iZ_end = 1;
	}
	else {
		iZ_start = -4; iZ_end = 5;
	}

	N0_forNumberDensity = 0.0;
	N0_forGradient = 0.0;
	N0_forLaplacian = 0.0;
	Lambda = 0.0;
	xi = 0.0;  yi = 0.0;  zi = 0.0;

	for (iX = -4; iX < 5; iX++) {
		for (iY = -4; iY < 5; iY++) {
			for (iZ = iZ_start; iZ < iZ_end; iZ++) {
				if (((iX == 0) && (iY == 0)) && (iZ == 0))continue;
				xj = PARTICLE_DISTANCE * (float)(iX);
				yj = PARTICLE_DISTANCE * (float)(iY);
				zj = PARTICLE_DISTANCE * (float)(iZ);
				distance2 = (xj - xi) * (xj - xi) + (yj - yi) * (yj - yi) + (zj - zi) * (zj - zi);
				distance = sqrt(distance2);
				N0_forNumberDensity += weight(distance, Re_forNumberDensity);
				N0_forGradient += weight(distance, Re_forGradient);
				N0_forLaplacian += weight(distance, Re_forLaplacian);
				Lambda += distance2 * weight(distance, Re_forLaplacian);
			}
		}
	}
	Lambda = Lambda / N0_forLaplacian;
}


float weight(float distance, float re) {
	float weightIJ;

	if (distance >= re) {
		weightIJ = 0.0;
	}
	else {
		weightIJ = (re / distance) - 1.0;
	}
	return weightIJ;
}


void mainLoopOfSimulation(void) {

	calGravity(); //重力の計算 (動くものには 0,-9.8, 0 動かないものには 0,0,0) を入れる
	calViscosity();
	moveParticle();
	collision();
	calPressure();
	calPressureGradient();
	moveParticleUsingPressureGradient();

	Time_ += DT;
}

// 重力の計算 (動くものには 0,-9.8, 0 動かないものには 0,0,0) を入れる
void calGravity(void) {
	int i;

	for (i = 0; i < NumberOfParticles; i++) {
		if (ParticleType[i] == FLUID) {
			Acceleration[i * 3] = G_X;
			Acceleration[i * 3 + 1] = G_Y;
			Acceleration[i * 3 + 2] = G_Z;
		}
		else {
			Acceleration[i * 3] = 0.0;
			Acceleration[i * 3 + 1] = 0.0;
			Acceleration[i * 3 + 2] = 0.0;
		}
	}
}


// 粘着項の計算 λ▽2u
void calViscosity(void) {
	int i, j;
	float viscosityTerm_x, viscosityTerm_y, viscosityTerm_z;
	float distance, distance2;
	float w;
	float xij, yij, zij;
	float a;

	a = (KINEMATIC_VISCOSITY) * (2.0 * DIM) / (N0_forLaplacian * Lambda);
	for (i = 0; i < NumberOfParticles; i++) {
		if (ParticleType[i] != FLUID) continue;
		viscosityTerm_x = 0.0;  viscosityTerm_y = 0.0;  viscosityTerm_z = 0.0;

		for (j = 0; j < NumberOfParticles; j++) {
			if ((j == i) || (ParticleType[j] == GHOST)) continue;
			xij = Position[j * 3] - Position[i * 3];
			yij = Position[j * 3 + 1] - Position[i * 3 + 1];
			zij = Position[j * 3 + 2] - Position[i * 3 + 2];
			distance2 = (xij * xij) + (yij * yij) + (zij * zij);
			distance = sqrt(distance2);
			if (distance < Re_forLaplacian) {
				w = weight(distance, Re_forLaplacian);
				viscosityTerm_x += (Velocity[j * 3] - Velocity[i * 3]) * w;
				viscosityTerm_y += (Velocity[j * 3 + 1] - Velocity[i * 3 + 1]) * w;
				viscosityTerm_z += (Velocity[j * 3 + 2] - Velocity[i * 3 + 2]) * w;
			}
		}
		viscosityTerm_x = viscosityTerm_x * a;
		viscosityTerm_y = viscosityTerm_y * a;
		viscosityTerm_z = viscosityTerm_z * a;
		Acceleration[i * 3] += viscosityTerm_x;
		Acceleration[i * 3 + 1] += viscosityTerm_y;
		Acceleration[i * 3 + 2] += viscosityTerm_z;
	}
}


void moveParticle(void) {
	int i;

	for (i = 0; i < NumberOfParticles; i++) {
		if (ParticleType[i] == FLUID) {
			Velocity[i * 3] += Acceleration[i * 3] * DT;
			Velocity[i * 3 + 1] += Acceleration[i * 3 + 1] * DT;
			Velocity[i * 3 + 2] += Acceleration[i * 3 + 2] * DT;

			Position[i * 3] += Velocity[i * 3] * DT;
			Position[i * 3 + 1] += Velocity[i * 3 + 1] * DT;
			Position[i * 3 + 2] += Velocity[i * 3 + 2] * DT;
		}
		Acceleration[i * 3] = 0.0;
		Acceleration[i * 3 + 1] = 0.0;
		Acceleration[i * 3 + 2] = 0.0;
	}
}


void collision(void) {
	int    i, j;
	float xij, yij, zij;
	float distance, distance2;
	float forceDT; /* forceDT is the impulse of collision between particles */
	float mi, mj;
	float velocity_ix, velocity_iy, velocity_iz;
	float e = COEFFICIENT_OF_RESTITUTION;
	static float VelocityAfterCollision[3 * ARRAY_SIZE];

	for (i = 0; i < 3 * NumberOfParticles; i++) {
		VelocityAfterCollision[i] = Velocity[i];
	}
	for (i = 0; i < NumberOfParticles; i++) {
		if (ParticleType[i] == FLUID) {
			mi = FluidDensity;
			velocity_ix = Velocity[i * 3];
			velocity_iy = Velocity[i * 3 + 1];
			velocity_iz = Velocity[i * 3 + 2];
			for (j = 0; j < NumberOfParticles; j++) {
				if ((j == i) || (ParticleType[j] == GHOST)) continue;
				xij = Position[j * 3] - Position[i * 3];
				yij = Position[j * 3 + 1] - Position[i * 3 + 1];
				zij = Position[j * 3 + 2] - Position[i * 3 + 2];
				distance2 = (xij * xij) + (yij * yij) + (zij * zij);
				if (distance2 < collisionDistance2) {
					distance = sqrt(distance2);
					forceDT = (velocity_ix - Velocity[j * 3]) * (xij / distance)
						+ (velocity_iy - Velocity[j * 3 + 1]) * (yij / distance)
						+ (velocity_iz - Velocity[j * 3 + 2]) * (zij / distance);
					if (forceDT > 0.0) {
						mj = FluidDensity;
						forceDT *= (1.0 + e) * mi * mj / (mi + mj);
						velocity_ix -= (forceDT / mi) * (xij / distance);
						velocity_iy -= (forceDT / mi) * (yij / distance);
						velocity_iz -= (forceDT / mi) * (zij / distance);
						/*
						if(j>i){ fprintf(stderr,"WARNING: Collision occured between %d and %d particles.\n",i,j); }
						*/
					}
				}
			}
			VelocityAfterCollision[i * 3] = velocity_ix;
			VelocityAfterCollision[i * 3 + 1] = velocity_iy;
			VelocityAfterCollision[i * 3 + 2] = velocity_iz;
		}
	}
	for (i = 0; i < NumberOfParticles; i++) {
		if (ParticleType[i] == FLUID) {
			Position[i * 3] += (VelocityAfterCollision[i * 3] - Velocity[i * 3]) * DT;
			Position[i * 3 + 1] += (VelocityAfterCollision[i * 3 + 1] - Velocity[i * 3 + 1]) * DT;
			Position[i * 3 + 2] += (VelocityAfterCollision[i * 3 + 2] - Velocity[i * 3 + 2]) * DT;
			Velocity[i * 3] = VelocityAfterCollision[i * 3];
			Velocity[i * 3 + 1] = VelocityAfterCollision[i * 3 + 1];
			Velocity[i * 3 + 2] = VelocityAfterCollision[i * 3 + 2];
		}
	}
}


void calPressure(void) {
	calNumberDensity();
	setBoundaryCondition();
	setSourceTerm();
	setMatrix();
	solveSimultaniousEquationsByGaussEliminationMethod();
	removeNegativePressure();
	setMinimumPressure();
}


void calNumberDensity(void) {
	int    i, j;
	float xij, yij, zij;
	float distance, distance2;
	float w;

	for (i = 0; i < NumberOfParticles; i++) {
		NumberDensity[i] = 0.0;
		if (ParticleType[i] == GHOST) continue;
		for (j = 0; j < NumberOfParticles; j++) {
			if ((j == i) || (ParticleType[j] == GHOST)) continue;
			xij = Position[j * 3] - Position[i * 3];
			yij = Position[j * 3 + 1] - Position[i * 3 + 1];
			zij = Position[j * 3 + 2] - Position[i * 3 + 2];
			distance2 = (xij * xij) + (yij * yij) + (zij * zij);
			distance = sqrt(distance2);
			w = weight(distance, Re_forNumberDensity);
			NumberDensity[i] += w;
		}
	}
}


void setBoundaryCondition(void) {
	int i;
	float n0 = N0_forNumberDensity;
	float beta = THRESHOLD_RATIO_OF_NUMBER_DENSITY;

	for (i = 0; i < NumberOfParticles; i++) {
		if (ParticleType[i] == GHOST || ParticleType[i] == DUMMY_WALL) {
			BoundaryCondition[i] = GHOST_OR_DUMMY;
		}
		else if (NumberDensity[i] < beta * n0) {
			BoundaryCondition[i] = SURFACE_PARTICLE;
			//BoundaryCondition[i] = INNER_PARTICLE;
		}
		else {
			BoundaryCondition[i] = INNER_PARTICLE;
		}
	}
}


void setSourceTerm(void) {
	int i;
	float n0 = N0_forNumberDensity;
	float gamma = RELAXATION_COEFFICIENT_FOR_PRESSURE;

	for (i = 0; i < NumberOfParticles; i++) {
		SourceTerm[i] = 0.0;
		if (ParticleType[i] == GHOST || ParticleType[i] == DUMMY_WALL) continue;
		if (BoundaryCondition[i] == INNER_PARTICLE) {
			SourceTerm[i] = gamma * (1.0 / (DT * DT)) * ((NumberDensity[i] - n0) / n0);
		}
		else if (BoundaryCondition[i] == SURFACE_PARTICLE) {
			SourceTerm[i] = 0.0;
		}
	}
}


void setMatrix(void) {
	float xij, yij, zij;
	float distance, distance2;
	float coefficientIJ;
	float n0 = N0_forLaplacian;
	int    i, j;
	float a;
	int n = NumberOfParticles;

	for (i = 0; i < NumberOfParticles; i++) {
		for (j = 0; j < NumberOfParticles; j++) {
			CoefficientMatrix[i * n + j] = 0.0;
		}
	}

	{
		//a = 2.0 * DIM / (n0 * Lambda);
		//for (i = 0; i < NumberOfParticles; i++) {
		//    if (BoundaryCondition[i] != INNER_PARTICLE) continue;
		//    for (j = 0; j < NumberOfParticles; j++) {
		//        if ((j == i) || (BoundaryCondition[j] == GHOST_OR_DUMMY)) continue;
		//        xij = Position[j * 3] - Position[i * 3];
		//        yij = Position[j * 3 + 1] - Position[i * 3 + 1];
		//        zij = Position[j * 3 + 2] - Position[i * 3 + 2];
		//        distance2 = (xij * xij) + (yij * yij) + (zij * zij);
		//        distance = sqrt(distance2);
		//        if (distance >= Re_forLaplacian)continue;
		//        coefficientIJ = a * weight(distance, Re_forLaplacian) / FluidDensity;
		//        CoefficientMatrix[i * n + j] = (-1.0) * coefficientIJ;
		//        CoefficientMatrix[i * n + i] += coefficientIJ;
		//    }
		//    CoefficientMatrix[i * n + i] += (COMPRESSIBILITY) / (DT * DT);
		//}
		//exceptionalProcessingForBoundaryCondition();
	}
	{
		a = -1 / FluidDensity * 2.0 * DIM / (n0 * Lambda); // -(1/p)(2d/λn0)
		for (i = 0; i < NumberOfParticles; i++) {
			if (BoundaryCondition[i] != INNER_PARTICLE) continue; //内部粒子のみ計算
			for (j = 0; j < NumberOfParticles; j++) {
				if ((j == i) || (BoundaryCondition[j] == GHOST_OR_DUMMY)) continue; //M._iiはiから見た他の重み*定数の総和でありijで増やしていっているため個々としては計算しない
				xij = Position[j * 3] - Position[i * 3];
				yij = Position[j * 3 + 1] - Position[i * 3 + 1];
				zij = Position[j * 3 + 2] - Position[i * 3 + 2];
				distance2 = (xij * xij) + (yij * yij) + (zij * zij);
				distance = sqrt(distance2);
				if (distance >= Re_forLaplacian)continue;
				coefficientIJ = a * weight(distance, Re_forLaplacian);
				CoefficientMatrix[i * n + j] = coefficientIJ;
				CoefficientMatrix[i * n + i] += -coefficientIJ; //M._iiに重みの総和を保存していく 符号反転を忘れないように
			}
			CoefficientMatrix[i * n + i] += (COMPRESSIBILITY) / (DT * DT);
		}
		exceptionalProcessingForBoundaryCondition(); //例外処理
	}
}


void exceptionalProcessingForBoundaryCondition(void) {
	/* If tere is no Dirichlet boundary condition on the fluid,
	   increase the diagonal terms of the matrix for an exception. This allows us to solve the matrix without Dirichlet boundary conditions. */
	checkBoundaryCondition();
	increaseDiagonalTerm();
}


void checkBoundaryCondition(void) {
	int i, j, count;
	float xij, yij, zij, distance2;

	for (i = 0; i < NumberOfParticles; i++) {
		if (BoundaryCondition[i] == GHOST_OR_DUMMY) {
			FlagForCheckingBoundaryCondition[i] = GHOST_OR_DUMMY;
		}
		else if (BoundaryCondition[i] == SURFACE_PARTICLE) {
			FlagForCheckingBoundaryCondition[i] = DIRICHLET_BOUNDARY_IS_CONNECTED;
		}
		else {
			FlagForCheckingBoundaryCondition[i] = DIRICHLET_BOUNDARY_IS_NOT_CONNECTED;
		}
	}

	do {
		count = 0;
		for (i = 0; i < NumberOfParticles; i++) {
			if (FlagForCheckingBoundaryCondition[i] == DIRICHLET_BOUNDARY_IS_CONNECTED) {
				for (j = 0; j < NumberOfParticles; j++) {
					if (j == i) continue;
					if ((ParticleType[j] == GHOST) || (ParticleType[j] == DUMMY_WALL)) continue;
					if (FlagForCheckingBoundaryCondition[j] == DIRICHLET_BOUNDARY_IS_NOT_CONNECTED) {
						xij = Position[j * 3] - Position[i * 3];
						yij = Position[j * 3 + 1] - Position[i * 3 + 1];
						zij = Position[j * 3 + 2] - Position[i * 3 + 2];
						distance2 = (xij * xij) + (yij * yij) + (zij * zij);
						if (distance2 >= Re2_forLaplacian)continue;
						FlagForCheckingBoundaryCondition[j] = DIRICHLET_BOUNDARY_IS_CONNECTED;
					}
				}
				FlagForCheckingBoundaryCondition[i] = DIRICHLET_BOUNDARY_IS_CHECKED;
				count++;
			}
		}
	} while (count != 0); /* This procedure is repeated until the all fluid or wall particles (which have Dirhchlet boundary condition in the particle group) are in the state of "DIRICHLET_BOUNDARY_IS_CHECKED".*/

	for (i = 0; i < NumberOfParticles; i++) {
		if (FlagForCheckingBoundaryCondition[i] == DIRICHLET_BOUNDARY_IS_NOT_CONNECTED) {
			fprintf(stderr, "WARNING: There is no dirichlet boundary condition for %d-th particle.\n", i);
		}
	}
}


void increaseDiagonalTerm(void) {
	int i;
	int n = NumberOfParticles;

	for (i = 0; i < n; i++) {
		if (FlagForCheckingBoundaryCondition[i] == DIRICHLET_BOUNDARY_IS_NOT_CONNECTED) {
			CoefficientMatrix[i * n + i] = 2.0 * CoefficientMatrix[i * n + i];
		}
	}
}


void solveSimultaniousEquationsByGaussEliminationMethod(void) {
	int    i, j, k;
	float c;
	float sumOfTerms;
	int    n = NumberOfParticles;

	for (i = 0; i < n; i++) {
		Pressure[i] = 0.0;
	}
	for (i = 0; i < n - 1; i++) {
		if (BoundaryCondition[i] != INNER_PARTICLE) continue;
		for (j = i + 1; j < n; j++) {
			if (BoundaryCondition[j] == GHOST_OR_DUMMY) continue;
			c = CoefficientMatrix[j * n + i] / CoefficientMatrix[i * n + i];
			for (k = i + 1; k < n; k++) {
				CoefficientMatrix[j * n + k] -= c * CoefficientMatrix[i * n + k];
			}
			SourceTerm[j] -= c * SourceTerm[i];
		}
	}
	for (i = n - 1; i >= 0; i--) {
		if (BoundaryCondition[i] != INNER_PARTICLE) continue;
		sumOfTerms = 0.0;
		for (j = i + 1; j < n; j++) {
			if (BoundaryCondition[j] == GHOST_OR_DUMMY) continue;
			sumOfTerms += CoefficientMatrix[i * n + j] * Pressure[j];
		}
		Pressure[i] = (SourceTerm[i] - sumOfTerms) / CoefficientMatrix[i * n + i];
	}
}


void removeNegativePressure(void) {
	int i;

	for (i = 0; i < NumberOfParticles; i++) {
		if (Pressure[i] < 0.0)Pressure[i] = 0.0;
	}
}


void setMinimumPressure(void) {
	float xij, yij, zij, distance2;
	int i, j;

	for (i = 0; i < NumberOfParticles; i++) {
		if (ParticleType[i] == GHOST || ParticleType[i] == DUMMY_WALL)continue;
		MinimumPressure[i] = Pressure[i];
		for (j = 0; j < NumberOfParticles; j++) {
			if ((j == i) || (ParticleType[j] == GHOST)) continue;
			if (ParticleType[j] == DUMMY_WALL) continue;
			xij = Position[j * 3] - Position[i * 3];
			yij = Position[j * 3 + 1] - Position[i * 3 + 1];
			zij = Position[j * 3 + 2] - Position[i * 3 + 2];
			distance2 = (xij * xij) + (yij * yij) + (zij * zij);
			if (distance2 >= Re2_forGradient)continue;
			if (MinimumPressure[i] > Pressure[j]) {
				MinimumPressure[i] = Pressure[j];
			}
		}
	}
}


void calPressureGradient(void) {
	int    i, j;
	float gradient_x, gradient_y, gradient_z;
	float xij, yij, zij;
	float distance, distance2;
	float w, pij;
	float a;

	//
	a = 1 / FluidDensity * DIM / N0_forGradient;
	for (i = 0; i < NumberOfParticles; i++) {
		if (ParticleType[i] != FLUID) continue;
		gradient_x = 0.0;  gradient_y = 0.0;  gradient_z = 0.0;
		for (j = 0; j < NumberOfParticles; j++) {
			if (j == i) continue;
			if (ParticleType[j] == GHOST) continue;
			if (ParticleType[j] == DUMMY_WALL) continue;
			xij = Position[j * 3] - Position[i * 3];
			yij = Position[j * 3 + 1] - Position[i * 3 + 1];
			zij = Position[j * 3 + 2] - Position[i * 3 + 2];
			distance2 = (xij * xij) + (yij * yij) + (zij * zij);
			distance = sqrt(distance2);
			if (distance < Re_forGradient) {
				w = weight(distance, Re_forGradient);
				pij = (Pressure[j] - MinimumPressure[i]) / distance2;
				gradient_x += xij * pij * w;
				gradient_y += yij * pij * w;
				gradient_z += zij * pij * w;
			}
		}
		gradient_x *= a;
		gradient_y *= a;
		gradient_z *= a;
		Acceleration[i * 3] = (-1.0) * gradient_x;
		Acceleration[i * 3 + 1] = (-1.0) * gradient_y;
		Acceleration[i * 3 + 2] = (-1.0) * gradient_z;
	}
}


void moveParticleUsingPressureGradient(void) {
	int i;

	for (i = 0; i < NumberOfParticles; i++) {
		if (ParticleType[i] == FLUID) {
			Velocity[i * 3] += Acceleration[i * 3] * DT;
			Velocity[i * 3 + 1] += Acceleration[i * 3 + 1] * DT;
			Velocity[i * 3 + 2] += Acceleration[i * 3 + 2] * DT;

			Position[i * 3] += Acceleration[i * 3] * DT * DT;
			Position[i * 3 + 1] += Acceleration[i * 3 + 1] * DT * DT;
			Position[i * 3 + 2] += Acceleration[i * 3 + 2] * DT * DT;
		}
		Acceleration[i * 3] = 0.0;
		Acceleration[i * 3 + 1] = 0.0;
		Acceleration[i * 3 + 2] = 0.0;
	}
}


#endif

#if USE_COMPUTE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PARTICLE_DISTANCE    0.5f / 5.1 //初期粒子間距離
#define EPS             (0.01 * PARTICLE_DISTANCE)
#define DIM 2				//次元数

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

int nP;
int count;
int cs_thread = 512;

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

static std::vector<Particle> particles;
static std::vector<Gameobject*> particle_gos;

void RdDat();
void compute_shader_init();

static std::shared_ptr<ComputeShader> density_CS;
static std::shared_ptr<ComputeShader> pressure_CS;
static std::shared_ptr<ComputeShader> collide_CS;
static std::shared_ptr<ComputeShader> force_CS;
static std::shared_ptr<ComputeShader> integrate_CS;

// bufffer
static Microsoft::WRL::ComPtr<ID3D11Buffer> particle_buffer;
static Microsoft::WRL::ComPtr<ID3D11Buffer> dencity_buffer;
static Microsoft::WRL::ComPtr<ID3D11Buffer> pressure_buffer;
static Microsoft::WRL::ComPtr<ID3D11Buffer> force_buffer;

static Microsoft::WRL::ComPtr<ID3D11Buffer> read_particle_buffer;

static Microsoft::WRL::ComPtr<ID3D11Buffer> particle_cb; //constantbuffer用
static Microsoft::WRL::ComPtr<ID3D11Buffer> particlerange_cb; //constantbuffer用

// SRV,UAV
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particle_read;
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dencity_read;
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pressure_read;
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> force_read;
static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_write;
static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> dencity_write;
static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pressure_write;
static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> force_write;

//template<class T>
//T* map_buffer_(Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer) {
//	HRESULT hr = S_OK;
//	//const D3D11_MAP map = D3D11_MAP_READ;
//	const D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
//	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
//	hr = Systems::DeviceContext->Map(buffer.Get(), 0, map, 0, &mappedBuffer);
//
//	if (FAILED(hr))
//	{
//		assert(0 && "failed Map InstanceBuffer dynamic(RenderManager)");
//		return nullptr;
//	}
//	return static_cast<T*>(mappedBuffer.pData);
//}
//void unmap_buffer_(Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer) {
//	Systems::DeviceContext->Unmap(buffer.Get(), 0);
//}

void Simulation_SPH::awake() {

	RdDat(); //粒子の配置
	compute_shader_init(); //compute_Shaderの初期化

	particle_gos.resize(nP);
	for (int i = 0; i < nP; i++) {

		particle_gos[i] = Gameobject_manager::createSphere("particle");
		particle_gos[i]->transform->local_pos = Vector3(particles[i].position);
		particle_gos[i]->transform->local_scale = Vector3((2.1 * PARTICLE_DISTANCE) * 0.3f);

		gameobject->add_child(particle_gos[i]);
	}

}

static float rotate = 0;
void Simulation_SPH::update() {
	Work_meter::start("simulation_SPH");

	if (input->getKeyState(Key::Left))rotate += time->deltaTime() * 0.2f;
	if (input->getKeyState(Key::Right))rotate -= time->deltaTime() * 0.2f;

	// コンスタントバッファーの更新
	{
		Systems::DeviceContext->CSSetConstantBuffers(0, 1, particle_cb.GetAddressOf());

		{
			ConstantBuffer_particlerange cb;
#ifdef USE_3D
			cb._Range_s = Vector3(-2, -4, -0.4f);
			cb._Range_g = Vector3(+2, +4, +0.4f);
#else
			cb._Range_s = Vector3(-8, -8, -0);
			cb._Range_g = Vector3(+8, +8, +0);
#endif
			cb._Range_noraml_0 = Vector3(cosf(rotate), sinf(rotate), 0);
			cb._Range_noraml_1 = Vector3(-sinf(rotate), cosf(rotate), 0);
			cb._Range_noraml_2 = Vector3(0, -0.1f, 1).unit_vect();

			Systems::DeviceContext->UpdateSubresource(particlerange_cb.Get(), 0, NULL, &cb, 0, 0);
			Systems::DeviceContext->CSSetConstantBuffers(1, 1, particlerange_cb.GetAddressOf());
		}
	}


	for (int i = 0; i < 3; ++i) {
		{
			ID3D11ShaderResourceView* pSRVs[1] = {
				particle_read.Get(),
			};
			ID3D11UnorderedAccessView* pUAVs[1] = {
				dencity_write.Get(),
			};

			density_CS->run(pSRVs, 1, pUAVs, 1, nP / cs_thread + 1, 1, 1);
		}
		{
			ID3D11ShaderResourceView* pSRVs[1] = {
				dencity_read.Get(),
			};
			ID3D11UnorderedAccessView* pUAVs[1] = {
				pressure_write.Get(),
			};

			pressure_CS->run(pSRVs, 1, pUAVs, 1, nP / cs_thread + 1, 1, 1);
		}
		{
			ID3D11ShaderResourceView* pSRVs[3] = {
				particle_read.Get(),
				dencity_read.Get(),
				pressure_read.Get(),
			};
			ID3D11UnorderedAccessView* pUAVs[1] = {
				force_write.Get()
			};

			force_CS->run(pSRVs, 3, pUAVs, 1, nP / cs_thread + 1, 1, 1);
		}
		//{
		//	ID3D11ShaderResourceView* pSRVs[2] = {
		//		particle_read.Get(),
		//		dencity_read.Get()
		//	};
		//	ID3D11UnorderedAccessView* pUAVs[1] = {
		//		force_write.Get(),
		//	};

		//	collide_CS->run(pSRVs, 2, pUAVs, 1, nP / cs_thread + 1, 1, 1);
		//}
		{
			ID3D11ShaderResourceView* pSRVs[1] = {
				force_read.Get()
			};
			ID3D11UnorderedAccessView* pUAVs[1] = {
				particle_write.Get(),
			};

			integrate_CS->run(pSRVs, 1, pUAVs, 1, nP / cs_thread + 1, 1, 1);
		}
	}

	Systems::DeviceContext->CopyResource(read_particle_buffer.Get(), particle_buffer.Get());
	{
		auto data = ComputeShader_function::map_buffer<Particle>(read_particle_buffer, Systems::DeviceContext);

		for (int i = 0; i < nP; ++i) {
			//data[i].position += Vector3(0, 1, 0) * 0.01f;
			particle_gos[i]->transform->local_pos = data[i].position;
		}
		ComputeShader_function::unmap_buffer(read_particle_buffer, Systems::DeviceContext);
	}


	Work_meter::stop("simulation_SPH");
}

void compute_shader_init() {
	count = (nP / cs_thread + 1) * cs_thread;

	density_CS = std::make_shared<ComputeShader>();
	pressure_CS = std::make_shared<ComputeShader>();
	force_CS = std::make_shared<ComputeShader>();
	collide_CS = std::make_shared<ComputeShader>();
	integrate_CS = std::make_shared<ComputeShader>();
	density_CS->Load("./DefaultShader/fluid_simulation_density_cs.cso");
	pressure_CS->Load("./DefaultShader/fluid_simulation_pressure_cs.cso");
	force_CS->Load("./DefaultShader/fluid_simulation_force_cs.cso");
	collide_CS->Load("./DefaultShader/fluid_simulation_collide.cso");
	integrate_CS->Load("./DefaultShader/fluid_simulation_integrate_cs.cso");

	// bufferの生成
	ComputeShader_function::create_StructureBuffer(sizeof(Particle), count, &particles[0], particle_buffer, false);
	ComputeShader_function::create_StructureBuffer(sizeof(ParticleDensity), count, nullptr, dencity_buffer, false);
	ComputeShader_function::create_StructureBuffer(sizeof(ParticlePressure), count, nullptr, pressure_buffer, false);
	ComputeShader_function::create_StructureBuffer(sizeof(ParticleForces), count, nullptr, force_buffer, false);

	ComputeShader_function::create_StructureBuffer(sizeof(Particle), count, nullptr, read_particle_buffer, true);

	// SRV,UAVの生成
	ComputeShader_function::createSRV_fromSB(particle_buffer, particle_read);
	ComputeShader_function::createUAV_fromSB(particle_buffer, particle_write);
	ComputeShader_function::createSRV_fromSB(dencity_buffer, dencity_read);
	ComputeShader_function::createUAV_fromSB(dencity_buffer, dencity_write);
	ComputeShader_function::createSRV_fromSB(pressure_buffer, pressure_read);
	ComputeShader_function::createUAV_fromSB(pressure_buffer, pressure_write);
	ComputeShader_function::createSRV_fromSB(force_buffer, force_read);
	ComputeShader_function::createUAV_fromSB(force_buffer, force_write);


	{
		float particleMass = 0.08f; //粒子質量
		float smoothlen = 0.5f; //粒子半径
		ConstantBuffer_particle cb;
		cb._NumParticles = nP;                       // 粒子数
		cb._TimeStep = 0.010f;                       // 時間刻み幅(dt)
		cb._Smoothlen = 0.5f;                      // 粒子半径
		cb._PressureStiffness = 0.57f;              // Beckerの係数 p0*c^2/7
		cb._WallStiffness = 2000.0f;                 // ペナルティ法の押し返す力
		cb._Viscosity = 3;                        // 粘性係数
		cb._Gravity = Vector3(0, -9.8f, 0);          // 重力

#ifdef USE_3D
		cb._RestDensity = 100;                   // 静止密度
		cb._DensityCoef = particleMass * 315.f / (64 * PI * pow(smoothlen, 9));      // 密度算出時の係数 weight { m * 315/(64 * PI * h^9) }  * (h^2 - r^2)^3 (3次元)
		cb._GradPressureCoef = particleMass * -45.0f / (PI * pow(smoothlen, 6));   // 圧力算出時の係数 ▽weight {m * -45/(PI * h^6)} * (h - r)^2 * r.normalize (3次元)
		cb._LapViscosityCoef = particleMass * 45.f / (PI * pow(smoothlen, 6)); // 粘性算出時の係数
#else
		cb._RestDensity = 4; // 静止密度
		cb._DensityCoef = particleMass * 4.f / (PI * pow(smoothlen, 8));      // 密度算出時の係数 weight { m * 4/(PI * h^8) }  * (h^2 - r^2)^3 (2次元)
		cb._GradPressureCoef = particleMass * -30.0f / (PI * pow(smoothlen, 5));   // 圧力算出時の係数 ▽weight {m * -30/(PI * h^5)} * (h - r)^2 * r.normalize (2次元)
		cb._LapViscosityCoef = particleMass * 20.f / (3 * PI * pow(smoothlen, 5)); // 粘性算出時の係数
#endif

		Systems::CreateConstantBuffer(particle_cb.ReleaseAndGetAddressOf(), sizeof(ConstantBuffer_particle));
		Systems::DeviceContext->UpdateSubresource(particle_cb.Get(), 0, NULL, &cb, 0, 0);
	}
	{
		Systems::CreateConstantBuffer(particlerange_cb.ReleaseAndGetAddressOf(), sizeof(ConstantBuffer_particlerange));
	}


}


void RdDat(void) {
	nP = 8001;

#if DIM == 3
	int iX, iY, iZ;
	int nX, nY, nZ;
	float x, y, z;
	int i = 0;
	int flagOfParticleGeneration;

	nX = (int)(1.0 / PARTICLE_DISTANCE) + 5;
	nY = (int)(0.6 / PARTICLE_DISTANCE) + 5;
	nZ = (int)(0.3 / PARTICLE_DISTANCE) + 5;
	for (iX = -4; iX < nX; iX++) {
		for (iY = -4; iY < nY; iY++) {
			for (iZ = -4; iZ < nZ; iZ++) {
				x = PARTICLE_DISTANCE * iX;
				y = PARTICLE_DISTANCE * iY;
				z = PARTICLE_DISTANCE * iZ;
				flagOfParticleGeneration = 0;

				/* dummy wall region */
				if ((((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 4.0 * PARTICLE_DISTANCE + EPS))) {
					Typ[i] = WLL;
					flagOfParticleGeneration = 1;
				}

				/* wall region */
				if ((((x > -2.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 2.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 2.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 2.0 * PARTICLE_DISTANCE + EPS))) {
					Typ[i] = WLL;
					flagOfParticleGeneration = 1;
				}

				/* wall region */
				if ((((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.6 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 4.0 * PARTICLE_DISTANCE + EPS))) {
					Typ[i] = WLL;
					flagOfParticleGeneration = 1;
				}

				/* empty region */
				if ((((x > 0.0 + EPS) && (x <= 1.00 + EPS)) && (y > 0.0 + EPS)) && ((z > 0.0 + EPS) && (z <= 0.3 + EPS))) {
					flagOfParticleGeneration = 0;
				}

				/* fluid region */
				if ((((x > 0.0 + EPS) && (x <= 0.25 + EPS)) && ((y > 0.0 + EPS) && (y < 0.5 + EPS))) && ((z > 0.0 + EPS) && (z <= 0.3 + EPS))) {
					Typ[i] = FLD;
					flagOfParticleGeneration = 1;
				}

				if (flagOfParticleGeneration == 1) {
					Pos[i * 3] = x;
					Pos[i * 3 + 1] = y;
					Pos[i * 3 + 2] = z;
					i++;
				}
			}
		}
	}
	nP = i;
	for (i = 0; i < nP * 3; i++) { Vel[i] = 0.0; }
#elif  DIM == 2
	// 各粒子の初期座標など
	int iX, iY, iZ;
	float x = 0, y = 0, z = 0;
	int i = 0;
	int flagOfParticleGeneration;

#ifdef USE_3D
	for (iX = 0; iX < 20; iX++) {
		for (iY = 0; iY < 80; iY++) {
			for (iZ = 0; iZ < 5; iZ++) {
				//for (iX = 0; iX < 4; iX++) {
				//	for (iY = 0; iY < 5; iY++) {
				//		for (iZ = 0; iZ < 4; iZ++) {
				x = PARTICLE_DISTANCE * (float)(iX)-10 * PARTICLE_DISTANCE;
				y = PARTICLE_DISTANCE * (float)(iY)-40 * PARTICLE_DISTANCE;
				z = PARTICLE_DISTANCE * (float)(iZ)-5 * PARTICLE_DISTANCE;
#else
	for (iX = 0; iX < 100; iX++) {
		for (iY = 0; iY < 80; iY++) {
			//for (iX = 0; iX < 10; iX++) {
			//	for (iY = 0; iY < 8; iY++) {
			x = PARTICLE_DISTANCE * (float)(iX)-50 * PARTICLE_DISTANCE;
			y = PARTICLE_DISTANCE * (float)(iY)-40 * PARTICLE_DISTANCE;
#endif

			Particle p;
			p.position = Vector3(x, y, z);
			p.velocity = Vector3(0);
			particles.emplace_back(p);
			i++;


#ifdef USE_3D
		}
#endif
		}
	}
nP = i;
#endif
}

#endif

#if USE_COMPUTE_22
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PARTICLE_DISTANCE    0.025f //初期粒子間距離
#define PARTICLE_RADIAS    PARTICLE_DISTANCE * 2.1f //粒子半径
#define EPS             (0.01f * PARTICLE_DISTANCE)
#define DIM 2				//次元数

#define NUM_TYP  2		//粒子の種類数
#define DNS0 1000		//流体粒子の密度
#define DT 0.0005f			//時間刻み幅
#define SND 22.0f			//音速
#define KNM_VSC (1.0E-6)	//動粘性係数
#define CRT_NUM 0.1f	//クーラン条件数
#define COL_RAT 0.2f		//接近した粒子の反発率
#define DST_LMT_RAT 0.5f	//これ以上の粒子間の接近を許さない距離の係数
#define G_X 0.0			//重力加速度のx成分
#define G_Y -9.8f			//重力加速度のy成分
#define G_Z 0.0			//重力加速度のz成分
#define WEI(dist, re) ((re/dist) - 1.0)	//重み関数

struct ConstantBuffer_particle {
	int _NumParticles;        // 粒子数
	float _TimeStep;          // 時間刻み幅(dt)
	float _Smoothlen;         // 粒子半径
	float _PressureStiffness; // Beckerの係数
	float _RestDensity;       // 静止密度
	float _DensityCoef;       // 密度算出時の係数
	float _GradPressureCoef;  // 圧力算出時の係数
	float _LapViscosityCoef;  // 粘性算出時の係数
	Vector3 _Range;            // シミュレーション空間
	float _WallStiffness;     // ペナルティ法の押し返す力
	Vector3 _Gravity;          // 重力
	float _Viscosity;         // 粘性係数
};

int nP;
int count;
int cs_thread = 512;

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

static std::vector<Particle> particles;
static std::vector<Gameobject*> particle_gos;

void RdDat();
void compute_shader_init();

static std::shared_ptr<ComputeShader> density_CS;
static std::shared_ptr<ComputeShader> pressure_CS;
static std::shared_ptr<ComputeShader> force_CS;
static std::shared_ptr<ComputeShader> integrate_CS;

// bufffer
static Microsoft::WRL::ComPtr<ID3D11Buffer> particle_buffer;
static Microsoft::WRL::ComPtr<ID3D11Buffer> dencity_buffer;
static Microsoft::WRL::ComPtr<ID3D11Buffer> pressure_buffer;
static Microsoft::WRL::ComPtr<ID3D11Buffer> force_buffer;

static Microsoft::WRL::ComPtr<ID3D11Buffer> read_particle_buffer;

static Microsoft::WRL::ComPtr<ID3D11Buffer> particle_cb; //constantbuffer用

// SRV,UAV
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particle_read;
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> dencity_read;
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pressure_read;
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> force_read;
static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_write;
static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> dencity_write;
static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pressure_write;
static Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> force_write;

template<class T>
T* map_buffer_(Microsoft::WRL::ComPtr<ID3D11Buffer> & buffer) {
	HRESULT hr = S_OK;
	//const D3D11_MAP map = D3D11_MAP_READ;
	const D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	hr = Systems::DeviceContext->Map(buffer.Get(), 0, map, 0, &mappedBuffer);

	if (FAILED(hr))
	{
		assert(0 && "failed Map InstanceBuffer dynamic(RenderManager)");
		return nullptr;
	}
	return static_cast<T*>(mappedBuffer.pData);
}
void unmap_buffer_(Microsoft::WRL::ComPtr<ID3D11Buffer> & buffer) {
	Systems::DeviceContext->Unmap(buffer.Get(), 0);
}

void Simulation_SPH::awake() {

	RdDat(); //粒子の配置
	compute_shader_init(); //compute_Shaderの初期化

	particle_gos.resize(nP);
	for (int i = 0; i < nP; i++) {

		particle_gos[i] = Gameobject_manager::createSphere("particle");
		particle_gos[i]->transform->local_pos = Vector3(particles[i].position);
		particle_gos[i]->transform->local_scale = Vector3((2.1 * PARTICLE_DISTANCE) * 0.4f);

		gameobject->add_child(particle_gos[i]);
	}

}

void Simulation_SPH::update() {
	Work_meter::start("simulation_SPH");

	Systems::DeviceContext->CSSetConstantBuffers(0, 1, particle_cb.GetAddressOf());

	{
		ID3D11ShaderResourceView* pSRVs[1] = {
			particle_read.Get(),
		};
		ID3D11UnorderedAccessView* pUAVs[1] = {
			dencity_write.Get(),
		};

		density_CS->run(pSRVs, 1, pUAVs, 1, nP / cs_thread + 1, 1, 1);
	}
	{
		ID3D11ShaderResourceView* pSRVs[1] = {
			dencity_read.Get(),
		};
		ID3D11UnorderedAccessView* pUAVs[1] = {
			pressure_write.Get(),
		};

		pressure_CS->run(pSRVs, 1, pUAVs, 1, nP / cs_thread + 1, 1, 1);
	}
	{
		ID3D11ShaderResourceView* pSRVs[3] = {
			particle_read.Get(),
			dencity_read.Get(),
			pressure_read.Get(),
		};
		ID3D11UnorderedAccessView* pUAVs[1] = {
			force_write.Get()
		};

		force_CS->run(pSRVs, 3, pUAVs, 1, nP / cs_thread + 1, 1, 1);
	}
	{
		ID3D11ShaderResourceView* pSRVs[1] = {
			force_read.Get()
		};
		ID3D11UnorderedAccessView* pUAVs[1] = {
			particle_write.Get(),
		};

		integrate_CS->run(pSRVs, 1, pUAVs, 1, nP / cs_thread + 1, 1, 1);
	}

	//pressure_CS->run(pSRVs, 4, pUAVs, 4, nP / 256 + 1, 0, 0);
	//force_CS->run(pSRVs, 4, pUAVs, 4, nP / 256 + 1, 0, 0);
	//integrate_CS->run(pSRVs, 4, pUAVs, 4, nP / 256 + 1, 0, 0);
	//density_CS-> run(particle_read.GetAddressOf(), 1, dencity_write.GetAddressOf(),  1, nP / 256 + 1, 0, 0);
	//pressure_CS->run(dencity_read.GetAddressOf(),  1, pressure_write.GetAddressOf(), 1, nP / 256 + 1, 0, 0);
	//force_CS->   run(pressure_read.GetAddressOf(), 1, force_write.GetAddressOf(),    1, nP / 256 + 1, 0, 0);
	//integrate_CS->run(force_read.GetAddressOf(),   1, particle_write.GetAddressOf(), 1, nP / 256 + 1, 0, 0);

	Systems::DeviceContext->CopyResource(read_particle_buffer.Get(), particle_buffer.Get());
	{
		auto data = map_buffer_<Particle>(read_particle_buffer);

		for (int i = 0; i < nP; ++i) {
			//data[i].position += Vector3(0, 1, 0) * 0.01f;
			particle_gos[i]->transform->local_pos = data[i].position;
		}
		unmap_buffer_(read_particle_buffer);
	}


	Work_meter::stop("simulation_SPH");
}

void compute_shader_init() {
	count = (nP / cs_thread + 1) * cs_thread;

	density_CS = std::make_shared<ComputeShader>();
	pressure_CS = std::make_shared<ComputeShader>();
	force_CS = std::make_shared<ComputeShader>();
	integrate_CS = std::make_shared<ComputeShader>();
	density_CS->Load("./DefaultShader/fluid_simulation_density_cs.cso");
	pressure_CS->Load("./DefaultShader/fluid_simulation_pressure_cs.cso");
	force_CS->Load("./DefaultShader/fluid_simulation_force_cs.cso");
	integrate_CS->Load("./DefaultShader/fluid_simulation_integrate_cs.cso");

	// bufferの生成
	ComputeShader_function::create_StructureBuffer(sizeof(Particle), count, &particles[0], particle_buffer, false);
	ComputeShader_function::create_StructureBuffer(sizeof(ParticleDensity), count, nullptr, dencity_buffer, false);
	ComputeShader_function::create_StructureBuffer(sizeof(ParticlePressure), count, nullptr, pressure_buffer, false);
	ComputeShader_function::create_StructureBuffer(sizeof(ParticleForces), count, nullptr, force_buffer, false);

	ComputeShader_function::create_StructureBuffer(sizeof(Particle), count, nullptr, read_particle_buffer, true);

	// SRV,UAVの生成
	ComputeShader_function::createSRV_fromSB(particle_buffer, particle_read);
	ComputeShader_function::createUAV_fromSB(particle_buffer, particle_write);
	ComputeShader_function::createSRV_fromSB(dencity_buffer, dencity_read);
	ComputeShader_function::createUAV_fromSB(dencity_buffer, dencity_write);
	ComputeShader_function::createSRV_fromSB(pressure_buffer, pressure_read);
	ComputeShader_function::createUAV_fromSB(pressure_buffer, pressure_write);
	ComputeShader_function::createSRV_fromSB(force_buffer, force_read);
	ComputeShader_function::createUAV_fromSB(force_buffer, force_write);

	Systems::CreateConstantBuffer(particle_cb.ReleaseAndGetAddressOf(), sizeof(ConstantBuffer_particle));

	float particleMass = 1; //粒子質量
	float smoothlen = 0.012f; //粒子半径

	ConstantBuffer_particle cb;
	cb._NumParticles = nP;                       // 粒子数
	//cb._TimeStep = 0;                          // 時間刻み幅(dt)
	cb._TimeStep = 0.005f;                       // 時間刻み幅(dt)
	cb._Smoothlen = 0.012f;                      // 粒子半径
	cb._PressureStiffness = 200.0f;              // Beckerの係数 p0*c^2/7
	cb._RestDensity = 1000.0f;                   // 静止密度
	cb._WallStiffness = 3000.0f;                 // ペナルティ法の押し返す力
	cb._Viscosity = 0.1f;                        // 粘性係数
	cb._Gravity = Vector3(0, -9.8f, 0);          // 重力
	cb._Range = Vector3(1, 1, 0);                // シミュレーション空間
	cb._DensityCoef = particleMass * 4.f / (PI * pow(smoothlen, 8));      // 密度算出時の係数 weight { m * 4/(PI * h^8) }  * (h^2 - r^2)^3 (2次元)
	cb._GradPressureCoef = particleMass * -30.0f / (PI * pow(smoothlen, 5));   // 圧力算出時の係数 ▽weight {m * -30/(PI * h^5)} * (h - r)^2 * r.normalize (2次元)
	cb._LapViscosityCoef = particleMass * 20.f / (3 * PI * pow(smoothlen, 5)); // 粘性算出時の係数

	// 粒子数密度 = 密度
	//float n0 = 0;
	//float smooth2 = cb._Smoothlen * cb._Smoothlen;
	//for (int xi = -4; xi < 5; ++xi) {
	//	for (int yi = -4; yi < 5; ++yi) {
	//		float x = PARTICLE_DISTANCE * (float)xi;
	//		float y = PARTICLE_DISTANCE * (float)yi;
	//		float dist2 = x * x + y * y;
	//		//float dist2 = x * x + y * y;
	//		if (dist2 <= smooth2) {
	//			if (dist2 == 0.0)continue;
	//			float dist = sqrt(dist2);
	//			n0 += cb._DensityCoef * (smooth2 - dist2) * (smooth2 - dist2) * (smooth2 - dist2);
	//		}
	//	}
	//}
	//cb._RestDensity = particleMass * n0 / (PI * cb._Smoothlen * cb._Smoothlen);
	//cb._PressureStiffness = cb._RestDensity * 22 * 22 / 7.0f;


	Systems::DeviceContext->UpdateSubresource(particle_cb.Get(), 0, NULL, &cb, 0, 0);
}


void RdDat(void) {
	nP = 17000;

#if DIM == 3
	int iX, iY, iZ;
	int nX, nY, nZ;
	float x, y, z;
	int i = 0;
	int flagOfParticleGeneration;

	nX = (int)(1.0 / PARTICLE_DISTANCE) + 5;
	nY = (int)(0.6 / PARTICLE_DISTANCE) + 5;
	nZ = (int)(0.3 / PARTICLE_DISTANCE) + 5;
	for (iX = -4; iX < nX; iX++) {
		for (iY = -4; iY < nY; iY++) {
			for (iZ = -4; iZ < nZ; iZ++) {
				x = PARTICLE_DISTANCE * iX;
				y = PARTICLE_DISTANCE * iY;
				z = PARTICLE_DISTANCE * iZ;
				flagOfParticleGeneration = 0;

				/* dummy wall region */
				if ((((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 4.0 * PARTICLE_DISTANCE + EPS))) {
					Typ[i] = WLL;
					flagOfParticleGeneration = 1;
				}

				/* wall region */
				if ((((x > -2.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 2.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.0 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 2.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 2.0 * PARTICLE_DISTANCE + EPS))) {
					Typ[i] = WLL;
					flagOfParticleGeneration = 1;
				}

				/* wall region */
				if ((((x > -4.0 * PARTICLE_DISTANCE + EPS) && (x <= 1.00 + 4.0 * PARTICLE_DISTANCE + EPS)) && ((y > 0.6 - 2.0 * PARTICLE_DISTANCE + EPS) && (y <= 0.6 + EPS))) && ((z > 0.0 - 4.0 * PARTICLE_DISTANCE + EPS) && (z <= 0.3 + 4.0 * PARTICLE_DISTANCE + EPS))) {
					Typ[i] = WLL;
					flagOfParticleGeneration = 1;
				}

				/* empty region */
				if ((((x > 0.0 + EPS) && (x <= 1.00 + EPS)) && (y > 0.0 + EPS)) && ((z > 0.0 + EPS) && (z <= 0.3 + EPS))) {
					flagOfParticleGeneration = 0;
				}

				/* fluid region */
				if ((((x > 0.0 + EPS) && (x <= 0.25 + EPS)) && ((y > 0.0 + EPS) && (y < 0.5 + EPS))) && ((z > 0.0 + EPS) && (z <= 0.3 + EPS))) {
					Typ[i] = FLD;
					flagOfParticleGeneration = 1;
				}

				if (flagOfParticleGeneration == 1) {
					Pos[i * 3] = x;
					Pos[i * 3 + 1] = y;
					Pos[i * 3 + 2] = z;
					i++;
				}
			}
		}
	}
	nP = i;
	for (i = 0; i < nP * 3; i++) { Vel[i] = 0.0; }
#elif  DIM == 2
	// 各粒子の初期座標など
	int iX, iY;
	int nX, nY;
	float x, y, z;
	int i = 0;
	int flagOfParticleGeneration;

	nX = (int)(2.0 / PARTICLE_DISTANCE) + 5;
	nY = (int)(1.0 / PARTICLE_DISTANCE) + 5;
	for (iX = -4; iX < nX; iX++) {
		for (iY = -4; iY < nY; iY++) {
			x = PARTICLE_DISTANCE * (float)(iX);
			y = PARTICLE_DISTANCE * (float)(iY);
			z = 0.0;
			flagOfParticleGeneration = 0;

			/* fluid region */
			if (
				((x > 0.0 + EPS) && (x <= 0.25 + EPS)) &&
				((y > 0.0 + EPS) && (y <= 0.25 + EPS))
				) {
				flagOfParticleGeneration = 1;
			}

			if (flagOfParticleGeneration == 1) {
				Particle p;
				p.position = Vector3(x, y, z) + Vector3(rand() % 100 * 0.00001f, rand() % 100 * 0.00001f, 0);
				//p.position = Vector3(x, y, z);
				p.velocity = Vector3(0);
				particles.emplace_back(p);
				i++;
			}
		}
	}
	nP = i;
#endif
}

#endif