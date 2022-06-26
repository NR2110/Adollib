#pragma once
#include "collider_shape.h"

namespace Adollib {
	//Box�N���X
	class Box : public Collider_shape {
	public:
		Physics_function::Vector3	center;//���S���W
		Physics_function::Vector3	rotate;//��]
		Physics_function::Vector3	size;//�傫��

		Box(Physics_function::ALP_Collider* l_ALPcollider_ptr) : center(Physics_function::Vector3(0)), rotate(Physics_function::Vector3(0)), size(Physics_function::Vector3(1,1,1)) {
			shape_tag = Physics_function::ALPCollider_shape_type::BOX;
			ALPcollider_ptr = l_ALPcollider_ptr;
		};

		void adapt_Colliderdata() override {
			local_position = center;
			local_orientation = quaternion_from_euler(rotate);
			local_scale = size;

		};

		void Update_hierarchy(int hierarchy_num) override {
			//float ave = (size.x + size.y + size.z) * 0.333333333f;
			////position
			//{
			//	float vec3[3] = { center.x,center.y,center.z };
			//	ImGui::DragFloat3((std::string("center##") + std::to_string(hierarchy_num)).c_str() , vec3, ave * 0.001f, 0, 0, "%.2f");
			//	center = Vector3(vec3[0], vec3[1], vec3[2]);
			//}
			////rotate
			//{
			//	float vec3[3] = { rotate.x, rotate.y, rotate.z };
			//	ImGui::DragFloat3((std::string("rotate##") + std::to_string(hierarchy_num)).c_str(), vec3, 0.1f, 0, 0, "%.2f");
			//	rotate = Vector3(vec3[0], vec3[1], vec3[2]);
			//}
			////scale
			//{
			//	float vec3[3] = { size.x,size.y, size.z };
			//	ImGui::DragFloat3((std::string("size##") + std::to_string(hierarchy_num)).c_str(), vec3, ave * 0.001f, 0, 0, "%.2f");
			//	size = Vector3(vec3[0], vec3[1], vec3[2]);
			//}
		};


		void update_dop14() override {
			dop14.pos = world_position();

			//�e���_�̃��[�J�����W
			Physics_function::Vector3 half[4]{
			{+world_scale().x, -world_scale().y, -world_scale().z},
			{+world_scale().x, -world_scale().y, +world_scale().z},
			{+world_scale().x, +world_scale().y, -world_scale().z},
			{+world_scale().x, +world_scale().y, +world_scale().z}
			};

			Physics_function::Quaternion WO = world_orientation();
			for (int i = 0; i < 4; i++) {
				half[i] = vector3_quatrotate(half[i], WO);
			}

			//DOP�̍X�V
			float max_len = 0;
			for (int i = 0; i < Physics_function::DOP::DOP_size; i++) {
				max_len = 0;
				for (int o = 0; o < 4; o++) {
					float dis = fabsf(vector3_dot(Physics_function::DOP::DOP_14_axis[i], half[o]));
					if (max_len < dis) {
						dop14.max[i] = +dis * 1.00000001f;//�m���ɂ��邽�߂�����Ƒ傫�߂ɂƂ�
						dop14.min[i] = -dis * 1.00000001f;//�m���ɂ��邽�߂�����Ƒ傫�߂ɂƂ�
						max_len = dis;
					}
				}

			}
		};

		const Physics_function::Matrix33 local_tensor() const override {
			const Physics_function::Vector3& Wsize = world_scale();
			Physics_function::Matrix33 ret;

			ret = Physics_function::matrix33_identity();
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