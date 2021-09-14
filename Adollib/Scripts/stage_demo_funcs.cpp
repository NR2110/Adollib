#include "stage_demo.h"

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Physics/joint.h"

namespace Adollib
{
	Gameobject* Stage_demo::set_tree(Vector3 pos, Vector3 scale, Vector3 rotate, Vector3 color) {
		Gameobject* tree = nullptr;
		tree = Gameobject_manager::create(GO_tag::None);

		Vector4 stem_color = Vector4(90, 47, 27, 255) / 255.0f;
		Vector4 reaf_color = Vector4(48, 188, 0, 255) / 255.0f;
		Vector4 floar_color = Vector4(188, 214, 54, 255) / 255.0f;

		tree->transform->local_pos = pos;
		tree->transform->local_scale = scale;
		tree->transform->local_orient = quaternion_from_euler(rotate);

		const int Tree_size = 7;
		Gameobject* tree_parts[Tree_size] = { nullptr };
		for (int i = 0; i < Tree_size; i++) {
			tree_parts[i] = Gameobject_manager::createCube(GO_tag::Box);
		}

		tree_parts[0]->transform->local_pos = Vector3(0, 7, 0);
		tree_parts[0]->transform->local_scale = Vector3(1, 6.9f, 1);
		tree_parts[0]->material->color = stem_color;

		tree_parts[1]->transform->local_pos = Vector3(0, 15, 0);
		tree_parts[1]->transform->local_scale = Vector3(1.5, 1, 1.5);
		tree_parts[1]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[1]->material->color = reaf_color;

		tree_parts[2]->transform->local_pos = Vector3(0, 13, 0);
		tree_parts[2]->transform->local_scale = Vector3(2.0, 1, 2.0);
		tree_parts[2]->transform->local_orient = quaternion_from_euler(0, 1.7f, 0);
		tree_parts[2]->material->color = reaf_color;

		tree_parts[3]->transform->local_pos = Vector3(0, 11, 0);
		tree_parts[3]->transform->local_scale = Vector3(3.0, 1, 3.0);
		tree_parts[3]->transform->local_orient = quaternion_from_euler(0, 13, 0);
		tree_parts[3]->material->color = reaf_color;

		tree_parts[4]->transform->local_pos = Vector3(0, 9, 0);
		tree_parts[4]->transform->local_scale = Vector3(3.5, 1, 3.5);
		tree_parts[4]->transform->local_orient = quaternion_from_euler(0, 28, 0);
		tree_parts[4]->material->color = reaf_color;

		tree_parts[5]->transform->local_pos = Vector3(0, 7, 0);
		tree_parts[5]->transform->local_scale = Vector3(4.5, 1, 4.5);
		tree_parts[5]->transform->local_orient = quaternion_from_euler(0, 14, 0);
		tree_parts[5]->material->color = reaf_color;

		tree_parts[6]->transform->local_pos = Vector3(0, 0.2f, 0);
		tree_parts[6]->transform->local_scale = Vector3(3, 0.2f, 3);
		tree_parts[6]->transform->local_orient = quaternion_from_euler(0, 0, 0);
		tree_parts[6]->material->color = floar_color;

		for (int i = 0; i < Tree_size; i++) {
			tree->add_child(tree_parts[i]);
		}

		Box* boxes[Tree_size];
		Collider* coll = tree->addComponent<Collider>();
		coll->physics_data.inertial_mass = 40;
		coll->tag = Collider_tags::Stage | Collider_tags::Kinematic_Stage | Collider_tags::Jumpable_Stage;

		for (int i = 0; i < Tree_size; i++) {
			boxes[i] = coll->add_shape<Box>();
		}

		boxes[0]->center = Vector3(0, 7, 0);
		boxes[0]->size = Vector3(1, 7, 1);

		boxes[1]->center = Vector3(0, 15, 0);
		boxes[1]->size = Vector3(1.5f, 1, 1.5f);
		boxes[1]->rotate = Vector3(0, 0, 0);

		boxes[2]->center = Vector3(0, 13, 0);
		boxes[2]->size = Vector3(2.0f, 1, 2.0f);
		boxes[2]->rotate = Vector3(0, 1.7f, 0);

		boxes[3]->center = Vector3(0, 11, 0);
		boxes[3]->size = Vector3(3.0f, 1, 3.0f);
		boxes[3]->rotate = Vector3(0, 13, 0);

		boxes[4]->center = Vector3(0, 9, 0);
		boxes[4]->size = Vector3(3.5f, 1, 3.5f);
		boxes[4]->rotate = Vector3(0, 28, 0);

		boxes[5]->center = Vector3(0, 7, 0);
		boxes[5]->size = Vector3(4.5f, 1, 4.5f);
		boxes[5]->rotate = Vector3(0, 14, 0);

		boxes[6]->center = Vector3(0, 0.2f, 0);
		boxes[6]->size = Vector3(3, 0.2f, 3);
		boxes[6]->rotate = Vector3(0, 0, 0);



		for (int i = 0; i < Tree_size; i++) {
			boxes[i]->adapt_Colliderdata();
		}
		Vector3 barycenter = coll->get_barycenter();
		for (int i = 0; i < Tree_size; i++) {
			boxes[i]->center -= barycenter;
			tree_parts[i]->transform->local_pos -= barycenter;
		}
		tree->transform->local_pos += barycenter;
		tree->transform->local_pos += barycenter * (scale.y - 1);

		return tree;
	}
}