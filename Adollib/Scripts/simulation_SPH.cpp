
#include "Simulation_SPH.h"
#include <unordered_map>

#include "../Adollib/Scripts/Object/gameobject_manager.h"

using namespace Adollib;

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

	double d = SPH_PDIST / SPH_SIMSCALE * 0.95;
	for (double x = INIT_MIN.x + d; x <= INIT_MAX.x - d; x += d)
		for (double y = INIT_MIN.y + d; y <= INIT_MAX.y - d; y += d)
			for (double z = INIT_MIN.z + d; z <= INIT_MAX.z - d; z += d)
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

#else

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
    double x, y, z;
    vec3() {
        x = 0; y = 0; z = 0;
    }
    vec3(const double _x, const double _y, const double _z) {
        x = _x; y = _y; z = _z;
    }
};

inline void operator+= (vec3& v, const vec3 a) {
    v.x += a.x; v.y += a.y; v.z += a.z;
}

inline void operator*= (vec3& v, const double a) {
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

inline vec3 operator* (const double a, const vec3 b)
{
    vec3 v;
    v.x = a * b.x;
    v.y = a * b.y;
    v.z = a * b.z;
    return v;
}

inline vec3 operator* (const vec3 a, const double b)
{
    return b * a;
}

inline vec3 operator/ (const vec3 a, const double b)
{
    if (b != 0)
        return a * (1 / b);
    else
    {
        std::cout << "error: division by 0." << std::endl;
        exit(1);
    }
}

inline double dot(const vec3 a, const vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline double mag2(const vec3 a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

inline double mag(const vec3 a)
{
    return sqrt(mag2(a));
}


/*----------------------------
  Constants
----------------------------*/

static const double SPH_RESTDENSITY = 600.0;
static const double SPH_INTSTIFF = 3.0;
static const double SPH_PMASS = 0.00020543;
static const double SPH_SIMSCALE = 0.004;
static const double H = 0.01;
//static const double PI = 3.141592653589793;
static const double DT = 0.004;
static const double SPH_VISC = 0.2;
static const double SPH_LIMIT = 200.0;
static const double SPH_RADIUS = 0.004;
static const double SPH_EPSILON = 0.00001;
static const double SPH_EXTSTIFF = 10000.0;
static const double SPH_EXTDAMP = 256.0;
static const double SPH_PDIST = pow(SPH_PMASS / SPH_RESTDENSITY,
    1.0 / 3.0);
static const vec3   MIN = vec3(0.0, 0.0, -10.0);
static const vec3   MAX = vec3(20.0, 50.0, 10.0);
static const vec3   INIT_MIN = vec3(0.0, 0.0, -10.0);
static const vec3   INIT_MAX = vec3(10.0, 20.0, 10.0);
static const double Poly6Kern = 315.0 / (64.0 * PI * pow(H, 9));
static const double SpikyKern = -45.0 / (PI * pow(H, 6));
static const double LapKern = 45.0 / (PI * pow(H, 6));


/*----------------------------
  Declaration of Particle
----------------------------*/

struct Particle
{
    vec3 pos, vel, f;
    double rho, prs;
    Gameobject* gameobject = nullptr; //描画用
};
typedef std::vector<Particle> Particles;

Particles* new_particles()
{
    Particles* p_ps;
    p_ps = new Particles();

    double d = SPH_PDIST / SPH_SIMSCALE * 0.95;
    for (double x = INIT_MIN.x + d; x <= INIT_MAX.x - d; x += d)
        for (double y = INIT_MIN.y + d; y <= INIT_MAX.y - d; y += d)
            for (double z = INIT_MIN.z + d; z <= INIT_MAX.z - d; z += d)
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
    double d = H / SPH_SIMSCALE;
    for (int x = -1; x < 2; x++)
        for (int y = -1; y < 2; y++)
            for (int z = -1; z < 2; z++)
            {
                vec3 v(r.x + (double)x * d, r.y + (double)y * d, r.z + (double)z * d);
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

NeighborMapIdx neighbor_map_idx(vec3 r)
{
    int x, y, z;
    int mx, my;
    double d;
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
    double H2, sum, r2, c;
    vec3 dr;
    ParticlePtrs ptrs;
    Particle* p_pj;

    H2 = H * H;

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
    double pterm, vterm, r, c;
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
    double speed, diff, adj;

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
    for (int i = 0; i < 5; i++)
    {
        output_particles(p_ps, i);
        simulation(p_ps);
    }
    delete_particles(p_ps);
}

#endif