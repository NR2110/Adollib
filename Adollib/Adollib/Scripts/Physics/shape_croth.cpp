#include "shape_croth.h"

#include "ALP__physics_manager.h"
#include "ALP__meshcoll_data.h"

using namespace Adollib;
using namespace Physics_function;

void Croth_vertex::effect_for_transform(Vector3& GO_Wposiiton, Quaternion& GO_Worientation, Vector3& GO_Wscale) {

	// transform‚Évertex‚Ì‰e‹¿‚ð“ü‚ê‚é
	GO_Wposiiton = GO_Wposiiton + vector3_quatrotate((mesh_data->vertices[vertex_id] + vertex_offset->at(vertex_id)) * GO_Wscale, GO_Worientation);
}
