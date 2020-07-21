#include "ALP_integrate.h"

using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region physics_function
//:::::::::::::::::::::::::::::::::::::::::::::::::::

using namespace physics_function;


//�O�͂ɂ�鑬�x�Ȃǂ̍X�V
void physics_function::applyexternalforce(std::vector<Collider*>& coll) {
	for (int i = 0; i < coll.size(); i++) {
		coll[i]->apply_external_force(timeStep);
	}
}
//:::::::::::::::::::::::::::


void physics_function::integrate(std::vector<Collider*>& coll) {
	for (int i = 0; i < coll.size(); i++) {
		coll[i]->integrate(timeStep);
	}
}

#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::