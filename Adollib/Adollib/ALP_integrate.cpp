#include "ALP_integrate.h"

using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region physics_function
//:::::::::::::::::::::::::::::::::::::::::::::::::::

using namespace physics_function;


//外力による速度などの更新
void physics_function::applyexternalforce(std::vector<Collider*>& coll) {
	for (int i = 0; i < coll.size(); i++) {
		//QUESTION : 渡す値と関数内での引数が違う
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