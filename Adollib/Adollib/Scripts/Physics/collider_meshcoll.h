#pragma once
#include "collider__base.h"
#include "meshcoll_resorce_manager.h"
#include "ALP__meshcoll_data.h"

namespace Adollib {
	
	//Mesh colliderクラス
	class Meshcoll : public Collider {
	public:
		Vector3 center;
		Vector3 rotate;
		Vector3 size;

	private:
		std::vector<physics_function::Meshcollider_mesh>* collider_mesh;

	public:
		//不動オブジェクトとして生成
		Meshcoll() :center(Vector3(0)), rotate(Vector3(0)), size(0) {}

		void load_mesh(const char* filename) {
			physics_function::Collider_ResourceManager::CreateMCFromFBX(filename, collider_mesh);
		}

		physics_function::Collider_data get_Colliderdata() const override {
			physics_function::Collider_data ret;

			ret.local_position = center;
			ret.local_orientation = quaternion_from_euler(rotate);
			ret.local_scale = size;

			ret.shape = physics_function::ALP_Collider_shape::Mesh;

			physics_function::Meshcoll_data data;
			data.collider_mesh = collider_mesh;

			ret.meshcoll_data = data;
			return ret;
		};

	};
}