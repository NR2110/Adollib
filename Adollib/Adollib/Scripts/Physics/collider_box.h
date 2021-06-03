#pragma once
#include "collider__base.h"
#include "collider_shape.h"

namespace Adollib {
	//Boxクラス
	class Box : public Collider_shape {
	public:
		Vector3	center;//中心座標
		Vector3	rotate;//回転
		Vector3	size;//大きさ

		Box(Physics_function::ALP_Collider* l_ALPcollider_ptr) : center(Vector3(0)), rotate(Vector3(0)), size(Vector3(1)) {
			shape_tag = Physics_function::ALPCollider_shape_type::BOX;
			tensor_type = Physics_function::Tensor_type::Box;
			ALPcollider_ptr = l_ALPcollider_ptr;
		};

		void adapt_Colliderdata() override {
			local_position = center;
			local_orientation = quaternion_from_euler(rotate);
			local_scale = size;

		};

		void Update_hierarchy() override {
			float ave = (size.x + size.y + size.z) * 0.333333333f;
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

			//各頂点のローカル座標
			Vector3 half[4]{
			{+world_scale().x, -world_scale().y, -world_scale().z},
			{+world_scale().x, -world_scale().y, +world_scale().z},
			{+world_scale().x, +world_scale().y, -world_scale().z},
			{+world_scale().x, +world_scale().y, +world_scale().z}
			};

			Quaternion WO = world_orientation();
			for (int i = 0; i < 4; i++) {
				half[i] = vector3_quatrotate(half[i], WO);
			}

			//DOPの更新
			float max_len = 0;
			for (int i = 0; i < DOP::DOP14_size; i++) {
				max_len = 0;
				for (int o = 0; o < 4; o++) {
					float dis = fabsf(vector3_dot(DOP::DOP_14_axis[i], half[o]));
					if (max_len < dis) {
						dop14.max[i] = +dis * 1.00000001f;//確実にするためちょっと大きめにとる
						dop14.min[i] = -dis * 1.00000001f;//確実にするためちょっと大きめにとる
						max_len = dis;
					}
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