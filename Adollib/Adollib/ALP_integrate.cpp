#include "ALP_integrate.h"

using namespace Adollib;
using namespace Contacts;

//:::::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region physics_function
//:::::::::::::::::::::::::::::::::::::::::::::::::::

using namespace physics_function;

//���x�Ȃǂ����Z�b�g
void physics_function::resetforce(std::list<Collider*>& coll) {

	std::list<Collider*>::iterator collitr;
	std::list<Collider*>::iterator collitr_end = coll.end();

	for (collitr = coll.begin(); collitr != collitr_end; collitr++) {
		(*collitr)->reset_force();
	}
}

//�O�͂ɂ�鑬�x�Ȃǂ̍X�V
void physics_function::applyexternalforce(std::list<std::shared_ptr<Adollib::Collider>>& coll) {

	std::list<std::shared_ptr<Adollib::Collider>>::iterator collitr;
	std::list<std::shared_ptr<Adollib::Collider>>::iterator collitr_end = coll.end();

	for (collitr = coll.begin(); collitr != collitr_end; collitr++) {
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