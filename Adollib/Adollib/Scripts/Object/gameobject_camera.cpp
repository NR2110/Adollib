#include "gameobject_camera.h"

namespace Adollib {

	void Camera::initialize() {

		std::list <std::shared_ptr<Component_camera>>::iterator itr = components.begin();
		std::list <std::shared_ptr<Component_camera>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->start();
		}

	}

	void Camera::update() {

		std::list <std::shared_ptr<Component_camera>>::iterator itr = components.begin();
		std::list <std::shared_ptr<Component_camera>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->update();
		}

	}


}