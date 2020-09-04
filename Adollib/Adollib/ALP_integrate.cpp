#include "ALP_integrate.h"

using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region physics_function
//:::::::::::::::::::::::::::::::::::::::::::::::::::

using namespace physics_function;


//外力による速度などの更新
void physics_function::applyexternalforce(std::list<std::shared_ptr<Adollib::Collider>>& coll) {

	std::list<std::shared_ptr<Adollib::Collider>>::iterator collitr;
	std::list<std::shared_ptr<Adollib::Collider>>::iterator collitr_end = coll.end();

	for (collitr = coll.begin(); collitr != collitr_end; collitr++) {
		//QUESTION : 渡す値と関数内での引数が違う
		(*collitr)->apply_external_force(physics_g::timeStep);
	}
}
//:::::::::::::::::::::::::::


void physics_function::integrate(std::list<std::shared_ptr<Adollib::Collider>>& coll) {

	std::list<std::shared_ptr<Adollib::Collider>>::iterator collitr;
	std::list<std::shared_ptr<Adollib::Collider>>::iterator collitr_end = coll.end();

	for (collitr = coll.begin(); collitr != collitr_end; collitr++) {
		(*collitr)->integrate(physics_g::timeStep);
	}
}

#pragma endregion
//:::::::::::::::::::::::::::::::::::::::::::::::::::