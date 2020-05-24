#pragma once

#include <list>
#include <memory>

#include "object.h"
#include "component.h"
#include "transform.h"
#include "scene_list.h"
#include "material.h"

namespace Adollib {
	class Gameobject : public object {
	private:
		struct ConstantBufferPerGO {
			DirectX::XMMATRIX world;
		};

		ComPtr<ID3D11Buffer> world_cb; //WVP�s��p�o�b�t�@

	public:
		std::string name = std::string("null"); //����go�̖��O(�����p)

		Transfome* transform = nullptr; //�`��p�̏��

		Material* material = nullptr;

		std::list < std::shared_ptr< RigitBody_Transform>> collider; //�A�^�b�`����Ă���collider

		std::list <std::shared_ptr<Component>> components; //�A�^�b�`����Ă���Conponent�̃|�C���^

		Gameobject* pearent = nullptr; //�e�ւ̃|�C���^
		std::list<std::shared_ptr<Gameobject>> children; //�ւ̃|�C���^

		bool active = true; //false�Ȃ�X�V�A�`����~�߂�

		Scenelist this_scene = Scenelist::scene_null; //����go�̂���scene

		std::list<std::shared_ptr<Gameobject>>::iterator go_iterator; //���g�ւ̃C�e���[�^�[(���g����?)

		//�A�^�b�`���ꂽ�R���|�[�l���g�̏���
		void initialize();
		void update();
		void render();
	private:


	public:
		//����go��world�ϊ��s���Ԃ�
		matrix get_word_matrix() {
			matrix WorldMatrix;
			if (pearent != nullptr) {
				WorldMatrix = (transform->prientation).get_rotate_matrix();
				WorldMatrix._41 = transform->position.x;
				WorldMatrix._42 = transform->position.y;
				WorldMatrix._43 = transform->position.z;
			}
			else {
				WorldMatrix = (transform->prientation * pearent->transform->prientation).get_rotate_matrix();
				WorldMatrix._41 = (transform->position + pearent->transform->position).x;
				WorldMatrix._42 = (transform->position + pearent->transform->position).y;
				WorldMatrix._43 = (transform->position + pearent->transform->position).z;
			}
			return WorldMatrix;
		};

		//go��world��ԏ�ł̂̎p����Ԃ�
		quaternion get_world_orientate() {
			if (pearent != nullptr) return pearent->transform->prientation * transform->prientation;
			else return transform->prientation;
		};
		//go��world��ԏ�ł̍��W��Ԃ�
		vector3 get_world_position() {
			if (pearent != nullptr) return pearent->transform->position + transform->position;
			else return transform->position;
		};

		//:::::::::::::::::::::::::
		// pos : �`����W����̑��΍��W
		// r : ���a
		//:::::::::::::::::::::::::
		void set_collider_sphere(vector3 pos, float r);

		//:::::::::::::::::::::::::
		// pos : �`����W����̑��΍��W
		// size : �`���]����� x,y,z �̑���half_size
		// rotate : �`���]����̑��Ή�]
		//:::::::::::::::::::::::::
		void set_collider_box(vector3 pos, vector3 size, quaternion orient);

		//:::::::::::::::::::::::::
		// pos : ���΍��W
		// normal : ���Ζ@��
		//:::::::::::::::::::::::::
		void set_collider_plane(vector3 pos, vector3 normal);

	};

}