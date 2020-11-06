#pragma once
#include "gameobject.h"
#include "gameobject_light.h"
#include "gameobject_camera.h"

#include "../Scene/scene.h"
#include "../Scene/scene_list.h"

#include <d3d11.h>
#include <WRL.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <map>


namespace Adollib {
	class Gameobject_manager{
	private:
		static Microsoft::WRL::ComPtr<ID3D11Buffer> light_cb;
		static Microsoft::WRL::ComPtr<ID3D11Buffer> view_cb;
		static Microsoft::WRL::ComPtr<ID3D11Buffer> projection_cb;

		static std::vector<object*> masters; //GO�e�q�c���[�̒��_��ۑ�
		static std::vector<object*> gos;	  //GO��1�̔z��ɕۑ�

		static std::thread physics_thread;
		static bool physics_thread_finish;

	public:
		//���̂͂��ׂĂ����ŕۑ�����
		static std::map<Scenelist, std::list<std::shared_ptr<Gameobject>>> gameobjects;
		//light
		static std::map<Scenelist, std::list<std::shared_ptr<Light>>> lights;
		//camera
		static std::map<Scenelist, std::list<std::shared_ptr<Camera>>> cameras;

		void awake();
		static void initialize(Scenelist Sce = Scene::now_scene);
		static void update(Scenelist Sce = Scene::now_scene);
		static void render(Scenelist Sce = Scene::now_scene);
		static void destroy(Scenelist Sce = Scene::now_scene);

		static Gameobject* create(Scenelist Sce = Scene::now_scene);
		static Gameobject* create(const u_int& go_name, Scenelist Sce = Scene::now_scene);

		static Camera* create_camera(Scenelist Sce = Scene::now_scene);
		static Camera* create_camera(const std::string go_name, Scenelist Sce = Scene::now_scene);

		static Light* create_light(Scenelist Sce = Scene::now_scene);
		static Light* create_light(const std::string go_name, Scenelist Sce = Scene::now_scene);

		//// =====================================================
		//// GameObject�𐶐����āA���̃|�C���^��Ԃ�(���O���w�肵�A���b�V���̎Q�Ƃ�����GameObject�𐶐�����B����create(const string&)���Ă�)
		//// =====================================================
		//// const string&	:	GameObject��name�ɓ���閼�O
		//// const string&	:	�g�p���郂�f���t�@�C���̃p�X
		//// =====================================================
		//// �߂�l GameObject*
		//// =====================================================
		//static Gameobject* create(const u_int& go_name, const std::string& model_filename, Scenelist Sce = Scene::now_scene);

		// ==============================================================
		// �v���~�e�B�u�I�u�W�F�N�g�ƓK�؂ȃR���C�_�[���쐬(Primitive.model�ւ̃t�@�C���p�X������)
		// ==============================================================
		// const string&		:	GameObject��name�ɓ���閼�O(�ȗ���)
		// bool					:	�������O��������
		// ==============================================================
		// �߂�l�@GameObject*	:	��������GameObject*��Ԃ�
		// ==============================================================

		static Gameobject* createFromFBX(const std::string& FBX_pass, const u_int = GO_tag::FBX, Scenelist Sce = Scene::now_scene);
		static Gameobject* createSphere(const u_int = GO_tag::Sphere, Scenelist Sce = Scene::now_scene);
		static Gameobject* createCylinder(const u_int = GO_tag::Cylinder, Scenelist Sce = Scene::now_scene);
		static Gameobject* createCube(const u_int = GO_tag::Box, Scenelist Sce = Scene::now_scene);
		static Gameobject* createPlane(const u_int = GO_tag::Plane, Scenelist Sce = Scene::now_scene);
		static Gameobject* createSprite(const u_int = GO_tag::Sprite, Scenelist Sce = Scene::now_scene);

		static Gameobject* find(const u_int name, Scenelist Sce = Scene::now_scene)
		{
			const auto itr_end = gameobjects[Sce].end();
			for (auto itr = gameobjects[Sce].begin(); itr != itr_end; itr++)
			{
				if (itr->get()->tag & name)
				{
					return itr->get();
				}
			}
			return nullptr;
		}
		static Camera* find_camera(const std::string& name, Scenelist Sce = Scene::now_scene)
		{
			const auto itr_end = cameras[Sce].end();
			for (auto itr = cameras[Sce].begin(); itr != itr_end; itr++)
			{
				if (itr->get()->name == name)
				{
					return itr->get();
				}
			}
			return nullptr;
		}
		static Light* find_light(const std::string& name, Scenelist Sce = Scene::now_scene)
		{
			const auto itr_end = lights[Sce].end();
			for (auto itr = lights[Sce].begin(); itr != itr_end; itr++)
			{
				if (itr->get()->name == name)
				{
					return itr->get();
				}
			}
			return nullptr;
		}


	};
}