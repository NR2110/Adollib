#pragma once
#include "collider_shape.h"

#include "../Imgui/imgui_all.h"
#include <memory>
#include <vector>

namespace Adollib {

	namespace Physics_function {
		class Meshcollider_data;

		// Croth頂点用クラス
		class Croth_vertex : public Collider_shape {
		public:
			Croth_vertex(Physics_function::ALP_Collider* l_ALPcollider_ptr) {
				shape_tag = Physics_function::ALPCollider_shape_type::Sphere;
				ALPcollider_ptr = l_ALPcollider_ptr;
			};

		public:
			Physics_function::Meshcollider_data* mesh_data = nullptr;
			int vertex_id = 0;
			std::shared_ptr<std::vector<Vector3>> vertex_offset;

		public:

			void adapt_Colliderdata() override {
				local_position = Vector3(0);
				local_orientation = quaternion_identity();
				local_scale = Vector3(1);
			}

			void Update_hierarchy() override {

			};


			void update_dop14() override {
				dop14.pos = world_position();
				for (int i = 0; i < DOP::DOP_size; i++) {
					dop14.max[i] = +world_scale().x * 1.0000001f;
					dop14.min[i] = -world_scale().x * 1.0000001f;
				}

			};

			const Matrix33 local_tensor() const override {
				const Vector3& Wsize = world_scale();
				Matrix33 ret;

				ret = matrix33_identity();
				ret._11 = 0.4f * Wsize.x * Wsize.x;
				ret._22 = 0.4f * Wsize.x * Wsize.x;
				ret._33 = 0.4f * Wsize.x * Wsize.x;
				return ret;
			};

			const float get_volume() const override {
				return 1.3333333f * DirectX::XM_PI * local_scale.x * local_scale.x * local_scale.x;
			}

			void effect_for_transform(Vector3& GO_Wposiiton, Quaternion& GO_Worientation, Vector3& GO_Wscale) override;
		};

	}
}