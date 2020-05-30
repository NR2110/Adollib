#include "gameobject.h"
#include "systems.h"
#include "rigit_body_manager.h"

#include "material_for_collider.h"
#include "Adollib.h"
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
		g_cb.world = matrix_world(transform->scale, transform->orientation.get_rotate_matrix(), transform->position).get_XMFLOAT4X4();
		Systems::DeviceContext->UpdateSubresource(world_cb.Get(), 0, NULL, &g_cb, 0, 0);
		Systems::DeviceContext->VSSetConstantBuffers(0, 1, world_cb.GetAddressOf());
		Systems::DeviceContext->PSSetConstantBuffers(0, 1, world_cb.GetAddressOf());

		//material->render();

		//if (Al_Global::render_collider_flag == false)  return;

		for (int i = 0;i < collider.size();i++) {
			Collider_renderer::render(collider[i]);
		}
		

	}



	void set_collider(Sphere* S, std::string tag, std::vector<std::string> no_hit_tag);
	void set_collider(Box* B, std::string tag, std::vector<std::string> no_hit_tag);
	void set_collider(Plane* P, std::string tag, std::vector<std::string> no_hit_tag);

	//:::::::::::::::::::::::::
	// pos : 描画座標からの相対座標
	// r : 半径
	//:::::::::::::::::::::::::
	void Gameobject::add_collider_sphere(vector3 pos, float r, float density, std::string tag, std::vector<std::string> no_hit_tag) {

		Sphere* sphere = new Sphere(r, density, pos); //衝突判定用の実体を生成
		sphere->gameobject = this;
		collider.push_back(sphere);
		Rigitbody_manager::set_rigitbody(sphere, this, tag, no_hit_tag, this_scene); //衝突判定のマネージャーに渡す

	}

	//:::::::::::::::::::::::::
	// pos : 描画座標からの相対座標
	// size : 描画回転からの x,y,z の相対half_size
	// rotate : 描画回転からの相対回転
	//:::::::::::::::::::::::::
	void Gameobject::add_collider_box(vector3 pos, vector3 size, quaternion orient, float density, std::string tag, std::vector<std::string> no_hit_tag) {

		Box* box = new Box(size, density, pos); //衝突判定用の実体を生成
		box->local_orientation = quaternion(1, 0, 0, 0);
		box->gameobject = this;
		collider.push_back(box);
		Rigitbody_manager::set_rigitbody(box, this, tag, no_hit_tag, this_scene); //衝突判定のマネージャーに渡す
	}

	//:::::::::::::::::::::::::
	// pos : 相対座標
	// normal : 相対法線
	//:::::::::::::::::::::::::
	void Gameobject::add_collider_plane(vector3 pos, vector3 normal, float density, std::string tag, std::vector<std::string> no_hit_tag) {

		float d = vector3_dot(normal, pos);
		Plane* plane = new Plane(normal, d);
		plane->gameobject = this;
		collider.push_back(plane);
		Rigitbody_manager::set_rigitbody(plane, this, tag, no_hit_tag, this_scene); //衝突判定のマネージャーに渡す
	}


}