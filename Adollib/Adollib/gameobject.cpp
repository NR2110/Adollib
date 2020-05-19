#include "gameobject.h"

namespace Adollib{

	void Gameobject::initialize() {

		std::list <std::shared_ptr<Component>>::iterator itr = components.begin();
		itr++;
		std::list <std::shared_ptr<Component>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->start();
		}

	}

	void Gameobject::update() {

		std::list <std::shared_ptr<Component>>::iterator itr = components.begin();
		itr++;
		std::list <std::shared_ptr<Component>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->update();
		}

	}

	void Gameobject::render() {

	

	}




}