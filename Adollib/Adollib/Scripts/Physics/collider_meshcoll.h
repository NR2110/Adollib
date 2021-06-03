#pragma once

#include "collider__base.h"
#include "meshcoll_resorce_manager.h"
#include "ALP__meshcoll_data.h"

namespace Adollib {

	//add_shape(Meshcoll.load_mesh)��mesh���A�^�b�`
	class Meshcoll {
	private:
		Physics_function::Meshcollider_data* collider_mesh = nullptr;
		std::string FBX_pass;

	public:

		std::vector<Physics_function::Meshcollider_data>* load_mesh(const char* filename) {
			return load_mesh(filename, true);
		};
		std::vector<Physics_function::Meshcollider_data>* load_mesh(const char* filename, bool Right_triangle) {
			//FBX��Load���s��
			std::vector<Physics_function::Meshcollider_data>* meshcoll_data = nullptr;
			Physics_function::Collider_ResourceManager::CreateMCFromFBX(filename, &meshcoll_data, Right_triangle);
			FBX_pass = filename; //���g��filepass�̕ۑ�

			return meshcoll_data;

			//Physics_function::Collider_shape coll_mesh;
			//coll_mesh.ALPcollider = ALPcollider_itr;

			//for (auto& data : *meshcoll_data) {
			//	coll_mesh.mesh_data = &data;
			//	ALPcollider_itr->shapes.emplace_back(coll_mesh);
			//}
		};
	};

	//add_shape(Meshcoll.load_mesh)��mesh�̓A�^�b�`���Ă��炤���߁AMeshcoll_part�͉B��
	namespace Physics_function {
		//Mesh collider�N���X
		class Meshcoll_part : public Collider_shape {
		public:
			Vector3 center;
			Vector3 rotate;
			Vector3 size;

			//Meshcoll_part���B����Ă���...�ƐM����
			Meshcollider_data* collider_mesh = nullptr;
			std::string FBX_pass;
		private:

		public:
			//�s���I�u�W�F�N�g�Ƃ��Đ���
			Meshcoll_part(Physics_function::ALP_Collider* l_ALPcollider_ptr, std::string FBX_pass, Meshcollider_data* data)
				:center(Vector3(0)), rotate(Vector3(0)), size(1) {
				shape_tag = Physics_function::ALPCollider_shape_type::Mesh;
				tensor_type = Physics_function::Tensor_type::Box;
				ALPcollider_ptr = l_ALPcollider_ptr;
			};

			void adapt_Colliderdata() override {
				local_position = center;
				local_orientation = quaternion_from_euler(rotate);
				local_scale = size;

			};

			void Update_hierarchy() override {
				float ave = 1;
				//position
				{
					float vec3[3] = { center.x,center.y,center.z };
					ImGui::DragFloat3("center", vec3, ave * 0.001f, 0, 0, "%.2f");
					center = Vector3(vec3[0], vec3[1], vec3[2]);
				}
				//rotate
				{
					float vec3[3] = { rotate.x, rotate.y, rotate.z };
					ImGui::DragFloat3("rotate", vec3, 0.1f, 0, 0, "%.2f");
					rotate = Vector3(vec3[0], vec3[1], vec3[2]);
				}
				//scale
				{
					float vec3[3] = { size.x,size.y, size.z };
					ImGui::DragFloat3("size", vec3, ave * 0.001f, 0, 0, "%.2f");
					size = Vector3(vec3[0], vec3[1], vec3[2]);
				}

			};

			void update_dop14() override {
				dop14.pos = world_position();

				//�e����min,max�̃��Z�b�g
				Vector3 rotated_axis[DOP::DOP14_size];
				for (int i = 0; i < DOP::DOP14_size; i++) {
					rotated_axis[i] = vector3_quatrotate(DOP::DOP_14_axis[i], world_orientation().inverse()).unit_vect();
					dop14.max[i] = -FLT_MAX;
					dop14.min[i] = +FLT_MAX;
				}

				for (int v_num = 0; v_num < 8; v_num++) {
					const Vector3& pos = mesh_data->base_pos[v_num] * world_scale();

					//DOP�̍X�V
					for (int i = 0; i < DOP::DOP14_size; i++) {
						const float dis = vector3_dot(rotated_axis[i], pos);
						if (dop14.min[i] > dis) dop14.min[i] = dis * 1.00000001f;//�m���ɂ��邽�߂�����Ƒ傫�߂ɂƂ�
						if (dop14.max[i] < dis) dop14.max[i] = dis * 1.00000001f;//�m���ɂ��邽�߂�����Ƒ傫�߂ɂƂ�

					}

				}
			};

			void update_inertial_tensor(Matrix& inertial_tensor, const float& inertial_mass) override {
				const Vector3& Wsize = world_scale();

				inertial_tensor = matrix_identity();
				inertial_tensor._11 = 0.3333333f * inertial_mass * ((Wsize.y * Wsize.y) + (Wsize.z * Wsize.z));
				inertial_tensor._22 = 0.3333333f * inertial_mass * ((Wsize.z * Wsize.z) + (Wsize.x * Wsize.x));
				inertial_tensor._33 = 0.3333333f * inertial_mass * ((Wsize.x * Wsize.x) + (Wsize.y * Wsize.y));
			};


		};


	}




}
