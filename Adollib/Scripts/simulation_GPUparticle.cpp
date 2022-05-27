
#include "simulation_GPUparticle.h"
#include <unordered_map>

#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Renderer/renderer_base.h"

#include "../Adollib/Scripts/Imgui/work_meter.h"

#include "../Adollib/Scripts/Renderer/Shader/compute_shader.h"

#include "../Adollib/Scripts/Main/systems.h"

using namespace Adollib;
using namespace Compute_S;

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PARTICLE_DISTANCE    0.5f / 5.1 //‰Šú—±qŠÔ‹——£
#define EPS             (0.01 * PARTICLE_DISTANCE)
#define DIM 2				//ŸŒ³”

int cs_thread = 512;

void Simulation_GPUparticle::awake() {


}

void Simulation_GPUparticle::update() {

}
