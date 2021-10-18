
#pragma once

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component.h"

#include "stage_types.h"
#include <unordered_map>

namespace Adollib {
	class Stage_base;
	class Player;

	class Stage_manager : public Component{

	public:
		static enum class Stage_types {
			demo,
			stage_0,
			stage_1,

			stagetype_max
		};

	private:
		bool is_changed_stage = false;

		Stage_types now_stage =  Stage_types::demo; //今のstagetype
		Stage_types next_stage = Stage_types::demo; //次のstagetype

		Vector3 player_respown_pos; //playerのrespownする座標

		std::unordered_map<Stage_types, Stage_base*> stages; //アタッチされたstageへのポインタ

		Player* player;

	private:
		template<typename T>
		T* add_stage(Stage_types stage_type) {
			// Componentクラスから派生したものかチェック
			static_assert(std::is_base_of<Stage_base, T>::value == true, "template T must inherit Stage_base");

			if (stages.count(stage_type) != 0)assert(0 && "dont attach same stage_type");

			Gameobject* GO = Gameobject_manager::create("Stage_type_");
			T* comp = GO->addComponent<T>();

			stages[stage_type] = comp;

			return comp;
		}

	public:
		void set_next_stage_type   (Stage_types next_stge); //next_stgeをsetする (次のstageに進む)
		Stage_base* get_current_stage() { return stages[now_stage]; };
		bool get_is_changed_stage() const { return is_changed_stage; };
	public:

		void awake() override;

		void start() override;

		void update() override;

	};

}