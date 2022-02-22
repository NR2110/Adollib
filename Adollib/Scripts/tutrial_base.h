#pragma once


namespace Adollib {

	class Gameobject;
	class Player;
	class Input_changer;

	class Tutrial_base {

	public:
		Gameobject* gameobject = nullptr;
		std::shared_ptr<Time> time = nullptr;
		Input_changer* input_changer = nullptr;
		Player* player = nullptr;
		float tutrial_ui_x = -63;

	public:

		virtual void awake() = 0;

		virtual void update() = 0;


	};

}