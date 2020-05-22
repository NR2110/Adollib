#pragma once

#include <list>
#include <memory>

#include "gameobject.h"
#include "component.h"
#include "transform.h"
#include "scene_list.h"

namespace Adollib {
	struct SPOTLIGHT {
		float index;
		float range;//���̓͂��͈�
		float type; //�L����������
		float near_area;
		float far_area;
		float dumy0;
		float dumy1;
		float dumy2;
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 dir;
	};

	struct POINTLIGHT {
		float index;
		float range;//���̓͂��͈�
		float type; //�L����������
		float dumy;
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 color;
	};

	class Light : public object {
	public:
		static const int POINTMAX = 32;
		static const int SPOTMAX = 32;

		vector4 LightDir;
		vector4 DirLightColor;
		vector4 Ambient;
		POINTLIGHT PointLight[POINTMAX];
		SPOTLIGHT SpotLight[SPOTMAX];

		void set_dirLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color);

		void set_ambient(DirectX::XMFLOAT3 amb);

		void set_pointLight(int index, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 color, float range);

		void set_spotLight(int index, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 dir, float range, float near, float far);


		std::string name = std::string("null"); //����go�̖��O(�����p)

		std::list <std::shared_ptr<Component>> components; //�A�^�b�`����Ă���Conponent�̃|�C���^

		Gameobject* pearent = nullptr; //�e�ւ̃|�C���^
		std::list<std::shared_ptr<Gameobject>> children; //�ւ̃|�C���^

		bool active = true; //false�Ȃ�X�V�A�`����~�߂�

		Scenelist this_scene = Scenelist::scene_null; //����go�̂���scene

		std::list<std::shared_ptr<Light>>::iterator go_iterator; //���g�ւ̃C�e���[�^�[(���g����?)

		//�A�^�b�`���ꂽ�R���|�[�l���g�̏���
		void initialize();
		void update();
		void render() {};
	private:

	public:

	};


}