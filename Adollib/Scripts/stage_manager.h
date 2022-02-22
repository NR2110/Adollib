
#pragma once

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component.h"

#include "stage_types.h"
#include <unordered_map>

namespace Adollib {
	class Stage_base;
	class Player_manager;

	class Stage_manager : public Component {

	public:
		bool is_changed_stage = false;

		Stage_types now_stage = Stage_types::demo; //����stagetype
		Stage_types next_stage = Stage_types::demo; //����stagetype
	private:
		float stage_timer = 0;

		Vector3 player_respown_pos; //player��respown������W

		std::unordered_map<Stage_types, Stage_base*> stages; //�A�^�b�`���ꂽstage�ւ̃|�C���^

		Player_manager* player;

	private:
		template<typename T>
		T* add_stage(Stage_types stage_type) {
			// Component�N���X����h���������̂��`�F�b�N
			static_assert(std::is_base_of<Stage_base, T>::value == true, "template T must inherit Stage_base");

			if (stages.count(stage_type) != 0)assert(0 && "dont attach same stage_type");

			Gameobject* GO = Gameobject_manager::create("Stage_type_");
			T* comp = GO->addComponent<T>();

			stages[stage_type] = comp;

			gameobject->add_child(GO);

			return comp;
		}

	public:
		void set_next_stage_type(Stage_types next_stge); //next_stge��set���� (����stage�ɐi��)
		Stage_base* get_current_stage() { return stages[now_stage]; };
		bool get_is_changed_stage() const { return is_changed_stage; };
		float get_nowstage_timer() { return stage_timer; }
	public:

		void awake() override;

		void start() override;

		void update() override;

	};

}