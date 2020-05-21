#pragma once

#include <map>
#include "gameobject.h"

#include "scene.h"
#include "scene_list.h"

namespace Adollib {
	class Gameobject_manager{
		

	private:

	public:
		//���̂͂��ׂĂ����ŕۑ�����
		static std::map<Scenelist, std::list<std::shared_ptr<Gameobject>>> gameobjects;

		void awake();
		static void initialize(Scenelist Sce = Scene::now_scene);
		static void update(Scenelist Sce = Scene::now_scene);
		static void render(Scenelist Sce = Scene::now_scene);
		static void destroy(Scenelist Sce = Scene::now_scene);

		static Gameobject* create();
		static Gameobject* create(const std::string go_name);

		// =====================================================
		// GameObject�𐶐����āA���̃|�C���^��Ԃ�(���O���w�肵�A���b�V���̎Q�Ƃ�����GameObject�𐶐�����B����create(const string&)���Ă�)
		// =====================================================
		// const string&	:	GameObject��name�ɓ���閼�O
		// const string&	:	�g�p���郂�f���t�@�C���̃p�X
		// =====================================================
		// �߂�l GameObject*
		// =====================================================
		static Gameobject* create(const std::string& go_name, const std::string& model_filename);

		// ==============================================================
		// �v���~�e�B�u�I�u�W�F�N�g�ƓK�؂ȃR���C�_�[���쐬(Primitive.model�ւ̃t�@�C���p�X������)
		// ==============================================================
		// const string&		:	GameObject��name�ɓ���閼�O(�ȗ���)
		// bool					:	�������O��������
		// ==============================================================
		// �߂�l�@GameObject*	:	��������GameObject*��Ԃ�
		// ==============================================================
		static Gameobject* createSphere(const std::string& go_name = "Sphere");
		static Gameobject* createCylinder(const std::string& go_name = "Cylinder");
		static Gameobject* createCube(const std::string& go_name = "Cube");
		static Gameobject* createPlane(const std::string& go_name = "Plane");
		static Gameobject* createSprite(const std::string& go_name = "Sprite");


	};
}