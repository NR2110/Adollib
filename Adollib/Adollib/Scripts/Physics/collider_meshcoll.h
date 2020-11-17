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
		std::vector<Physics_function::Meshcollider_mesh>* collider_mesh = nullptr;
		std::string FBX_pass;

	public:
		//不動オブジェクトとして生成
		Meshcoll() :center(Vector3(0)), rotate(Vector3(0)), size(1) {}

		void load_mesh(const char* filename) {
			Physics_function::Collider_ResourceManager::CreateMCFromFBX(filename, &collider_mesh);
			FBX_pass = filename;

			ALPcollider_itr->collider_meshes.clear();

			int coll_mesn_size = collider_mesh->size();

			Physics_function::ALP_Collider_mesh coll_mesh;
			coll_mesh.ALPcollider = ALPcollider_itr;
			auto itr = collider_mesh->begin();
			for(int i = 0;i < coll_mesn_size;i++){
				coll_mesh.mesh = itr;
				ALPcollider_itr->collider_meshes.emplace_back(coll_mesh);
				itr++;
			}

		}

		Physics_function::Collider_data get_Colliderdata() const override {
			Physics_function::Collider_data ret;

			ret.local_position = center;
			ret.local_orientation = quaternion_from_euler(rotate);
			ret.local_scale = size;

			ret.shape = Physics_function::ALP_Collider_shape::Mesh;

			Physics_function::Meshcoll_data data;
			//data.collider_mesh = collider_mesh;

			ret.meshcoll_data = data;
			return ret;
		};

	};
}