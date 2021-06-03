#pragma once

#ifdef _DEBUG
#define newD new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#include "vector.h"
#include "matrix.h"
#include "quaternion.h"
#include "math_e.h"

#include "crossing_func.h"
#include "closest_func.h"

#include "DOP.h"
