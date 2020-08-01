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
		//QUESTION : �n���l�Ɗ֐����ł̈������Ⴄ
		coll[i]->apply_external_force(physics_g::timeStep);
	}
}
//:::::::::::::::::::::::::::


void physics_function::integrate(std::vector<Collider*>& coll) {
	for (int i = 0; i < coll.size(); i++) {
		coll[i]->integrate(physics_g::timeStep);
	}
}

#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::