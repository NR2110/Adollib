#include "gameobject.h"
#include "systems.h"
#include "rigit_body_manager.h"
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

		//CB : ConstantBufferPerOBJ
		ConstantBufferPerGO g_cb;
		g_cb.world = get_word_matrix().get_XMFLOAT4X4();
		Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

		material->render();
	}



	void set_collider(Sphere* S, std::string tag, std::vector<std::string> no_hit_tag);
	void set_collider(Box* B, std::string tag, std::vector<std::string> no_hit_tag);
	void set_collider(Plane* P, std::string tag, std::vector<std::string> no_hit_tag);

	//:::::::::::::::::::::::::
	// pos : �`����W����̑��΍��W
	// r : ���a
	//:::::::::::::::::::::::::
	void Gameobject::set_collider_sphere(vector3 pos, float r, float density, std::string tag, std::vector<std::string> no_hit_tag) {

		Sphere* sphere = new Sphere(r, density, pos); //�Փ˔���p�̎��̂𐶐�
		Rigitbody_manager::set_rigitbody(sphere, this, tag, no_hit_tag, this_scene); //�Փ˔���̃}�l�[�W���[�ɓn��

	}

	//:::::::::::::::::::::::::
	// pos : �`����W����̑��΍��W
	// size : �`���]����� x,y,z �̑���half_size
	// rotate : �`���]����̑��Ή�]
	//:::::::::::::::::::::::::
	void Gameobject::set_collider_box(vector3 pos, vector3 size, quaternion orient, float density, std::string tag, std::vector<std::string> no_hit_tag) {

		Box* box = new Box(size, density, pos); //�Փ˔���p�̎��̂𐶐�
		box->local_orientation = quaternion(1, 0, 0, 0);
		Rigitbody_manager::set_rigitbody(box, this, tag, no_hit_tag, this_scene); //�Փ˔���̃}�l�[�W���[�ɓn��
	}

	//:::::::::::::::::::::::::
	// pos : ���΍��W
	// normal : ���Ζ@��
	//:::::::::::::::::::::::::
	void Gameobject::set_collider_plane(vector3 pos, vector3 normal, float density, std::string tag, std::vector<std::string> no_hit_tag) {

		float d = vector3_dot(normal, pos);
		Plane* plane = new Plane(normal, d);
		Rigitbody_manager::set_rigitbody(plane, this, tag, no_hit_tag, this_scene); //�Փ˔���̃}�l�[�W���[�ɓn��
	}


}