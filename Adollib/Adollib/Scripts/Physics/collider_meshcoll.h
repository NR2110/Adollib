#pragma once

#include "collider__base.h"
#include "meshcoll_resorce_manager.h"
#include "ALP__meshcoll_data.h"

namespace Adollib {
	
	//Mesh collider�N���X
	class Meshcoll : public Collider {
	public:
		Vector3 center;
		Vector3 rotate;
		Vector3 size;

	private:
		std::vector<physics_function::Meshcollider_mesh>* collider_mesh;

	public:
		//�s���I�u�W�F�N�g�Ƃ��Đ���
		Meshcoll() :center(Vector3(0)), rotate(Vector3(0)), size(1) {}

		void load_mesh(const char* filename) {
			physics_function::Collider_ResourceManager::CreateMCFromFBX(filename, &collider_mesh);

			ALPcollider_itr->collider_meshes.clear();

			int coll_mesn_size = collider_mesh->size();

			physics_function::ALP_Collider_mesh coll_mesh;
			coll_mesh.ALPcollider = ALPcollider_itr;
			auto& itr = collider_mesh->begin();
			for(int i = 0;i < coll_mesn_size;i++){
				coll_mesh.mesh = itr;
				ALPcollider_itr->collider_meshes.emplace_back(coll_mesh);
				itr++;
			}

		}

		physics_function::Collider_data get_Colliderdata() const override {
			physics_function::Collider_data ret;

			ret.local_position = center;
			ret.local_orientation = quaternion_from_euler(rotate);
			ret.local_scale = size;

			ret.shape = physics_function::ALP_Collider_shape::Mesh;

			physics_function::Meshcoll_data data;
			//data.collider_mesh = collider_mesh;

			ret.meshcoll_data = data;
			return ret;
		};

	};
}