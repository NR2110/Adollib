#include "gameobject.h"
#include "systems.h"
#include "ALP__manager.h"

#include "material_for_collider.h"
#include "Adollib.h"
#include "frustum_culling.h"

namespace Adollib{

	void Gameobject::initialize() {

		std::list <std::shared_ptr<Component>>::iterator itr = components.begin();
		std::list <std::shared_ptr<Component>>::iterator itr_end = components.end();

		Systems::CreateConstantBuffer(&world_cb, sizeof(ConstantBufferPerGO));

		for (; itr != itr_end; itr++) {
			itr->get()->start();
		}

	}

	void Gameobject::update() {

		std::list <std::shared_ptr<Component>>::iterator itr = components.begin();
		std::list <std::shared_ptr<Component>>::iterator itr_end = components.end();

		for (; itr != itr_end; itr++) {
			itr->get()->update();
		}



	}

	void Gameobject::render() {
		if (no_material == false) {

			//CB : ConstantBufferPerOBJ
			ConstantBufferPerGO g_cb;
			g_cb.world = matrix_world(transform->scale, transform->orientation.get_rotate_matrix(), transform->position).get_XMFLOAT4X4();
			Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
			Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
			Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

			//������J�����O�p
			FrustumCulling::update_obj(this);
			FrustumCulling::frustum_culling_init();

			material->render();
		}

		if (Al_Global::render_collider_flag == false)  return;

		for (int i = 0;i < collider.size();i++) {
			Collider_renderer::render(collider[i]);
		}
		

	}

	////:::::::::::::::::::::::::
	//// pos : �`����W����̑��΍��W
	//// r : ���a
	////:::::::::::::::::::::::::
	//Sphere* Gameobject::add_collider_sphere(vector3 pos, float r, float density, std::string tag, std::vector<std::string> no_hit_tag) {

	//	std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(r, density, pos); //�Փ˔���p�̎��̂𐶐�
	//	sphere->local_scale = vector3(1, 1, 1);
	//	sphere->local_orientation = quaternion_identity();
	//	sphere->gameobject = this;
	//	sphere->tag = tag;
	//	sphere->No_hit_tag = no_hit_tag;
	//	Collider* V = Rigitbody_manager::add_collider(this, this_scene); //�Փ˔���̃}�l�[�W���[�ɓn��
	//	return sphere.get();

	//}

	////:::::::::::::::::::::::::
	//// pos : �`����W����̑��΍��W
	//// size : �`���]����� x,y,z �̑���half_size
	//// rotate : �`���]����̑��Ή�]
	////:::::::::::::::::::::::::
	//Box* Gameobject::add_collider_box(vector3 pos, vector3 scale, quaternion orient, float density, std::string tag, std::vector<std::string> no_hit_tag) {

	//	std::shared_ptr<Box> box = std::make_shared<Box>(scale, density, pos); //�Փ˔���p�̎��̂𐶐�
	//	box->local_orientation = orient;
	//	box->local_scale = vector3(1, 1, 1);
	//	box->gameobject = this;
	//	collider.push_back(box);
	//	Rigitbody_manager::add_collider(box.get(), this, tag, no_hit_tag, this_scene); //�Փ˔���̃}�l�[�W���[�ɓn��
	//	return box.get();
	//}

	////:::::::::::::::::::::::::
	//// pos : ���΍��W
	//// normal : ���Ζ@��
	////:::::::::::::::::::::::::
	//Plane* Gameobject::add_collider_plane(vector3 pos, vector3 normal, float density, std::string tag, std::vector<std::string> no_hit_tag) {

	//	float d = vector3_dot(normal, pos);
	//	std::shared_ptr<Plane>  plane = std::make_shared<Plane>(normal, d);

	//	plane->move = false;
	//	plane->gameobject = this;
	//	plane->local_orientation = quaternion_from_to_rotate(vector3(0, 1, 0), normal);
	//	plane->local_scale = vector3(30, 0.1, 30);
	//	collider.push_back(plane);
	//	Rigitbody_manager::add_collider(plane.get(), this, tag, no_hit_tag, this_scene); //�Փ˔���̃}�l�[�W���[�ɓn��
	//	return plane.get();
	//}


}