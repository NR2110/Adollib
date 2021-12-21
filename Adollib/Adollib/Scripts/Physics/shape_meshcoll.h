#pragma once

#include "collider_shape.h"
#include "meshcoll_resorce_manager.h"
#include "ALP__meshcoll_data.h"

namespace Adollib {

	//add_shape(Meshcoll.load_mesh)でmeshはアタッチしてもらうため、Meshcoll_partは隠す
	namespace Physics_function {
		//Mesh colliderクラス
		class Meshcoll_part : public Collider_shape {
		public:
			Vector3 center;
			Vector3 rotate;
			Vector3 size;

			std::string FBX_pass;
		private:

		public:
			//不動オブジェクトとして生成
			Meshcoll_part(Physics_function::ALP_Collider* l_ALPcollider_ptr, std::string FBX_pass, Meshcollider_data* data)
				:center(Vector3(0)), rotate(Vector3(0)), size(1) {
				shape_tag = Physics_function::ALPCollider_shape_type::Mesh;
				ALPcollider_ptr = l_ALPcollider_ptr;
				mesh_data = data;
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

				//各軸のmin,maxのリセット
				Vector3 rotated_axis[DOP::DOP_size];
				for (int i = 0; i < DOP::DOP_size; i++) {
					rotated_axis[i] = vector3_quatrotate(DOP::DOP_14_axis[i], world_orientation().inverse()).unit_vect();
					dop14.max[i] = -FLT_MAX;
					dop14.min[i] = +FLT_MAX;
				}

				for (int v_num = 0; v_num < 8; v_num++) {
					const Vector3& pos = mesh_data->base_pos[v_num] * world_scale();

					//DOPの更新
					for (int i = 0; i < DOP::DOP_size; i++) {
						const float dis = vector3_dot(rotated_axis[i], pos);
						if (dop14.min[i] > dis) dop14.min[i] = dis * 1.00000001f;//確実にするためちょっと大きめにとる
						if (dop14.max[i] < dis) dop14.max[i] = dis * 1.00000001f;//確実にするためちょっと大きめにとる

					}

				}
			};

			const Matrix33 local_tensor() const override {
				const Vector3& Wsize = world_scale();
				Matrix33 ret;

				ret = matrix33_identity();
				ret._11 = 0.3333333f * ((Wsize.y * Wsize.y) + (Wsize.z * Wsize.z));
				ret._22 = 0.3333333f * ((Wsize.z * Wsize.z) + (Wsize.x * Wsize.x));
				ret._33 = 0.3333333f * ((Wsize.x * Wsize.x) + (Wsize.y * Wsize.y));
				return ret;
			};

			const float get_volume() const override {
				return local_scale.x * local_scale.y * local_scale.z;
			}


		};


	}




}
