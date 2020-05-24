#pragma once

#include <list>
#include <memory>

#include "gameobject.h"
#include "component.h"
#include "transform.h"
#include "scene_list.h"
#include "light_types.h"

namespace Adollib {

	class Light : public object {
	public:
		std::vector <std::shared_ptr<POINTLIGHT>> PointLight;
		std::vector <std::shared_ptr<SPOTLIGHT>>  SpotLight;

		void set_dirLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color);

		void set_ambient(DirectX::XMFLOAT3 amb);

		void set_pointLight(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 color, float range);

		void set_spotLight(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 dir, float range, float near, float far);


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