#pragma once

#include <list>
#include <memory>

#include "input.h"
#include "time.h"
#include "object.h"
#include "rigit_body.h"
#include "component.h"
#include "transform.h"
#include "scene_list.h"
#include "material.h"

namespace Adollib {
	class Gameobject : public object {
	private:
		struct ConstantBufferPerGO {
			DirectX::XMFLOAT4X4 world;
		};
		void update();
		ComPtr<ID3D11Buffer> world_cb; //WVP�s��p�o�b�t�@
	public:
		bool no_material = false; //material�����������Ă��邩

		std::string name = std::string("null"); //����go�̖��O(�����p)

		Material* material = nullptr;

		std::vector <Rigitbody*> collider; //�A�^�b�`����Ă���collider�ւ̃|�C���^

		std::list <std::shared_ptr<Component>> components; //�A�^�b�`����Ă���Conponent�̃|�C���^

		object* pearent = nullptr; //�e�ւ̃|�C���^
		object* get_pearent() {		//��Ԃ̐e��Ԃ�
			object* P = this;
			for (; P == nullptr;) {
				P = pearent;
			}
			return P;
		};

		std::list<std::shared_ptr<object>> children; //�ւ̃|�C���^
		std::list<std::shared_ptr<object>> get_children() {		//���ׂĂ̎q��Ԃ�
			std::list<std::shared_ptr<object>>::iterator itr = children.begin();
			std::list<std::shared_ptr<object>> ret;

			for (int i = 0; i < children.size(); i++) {
				ret.splice(ret.end(), itr->get()->get_children());
				itr++;
			}
			return ret;
		};

		bool active = true; //false�Ȃ�X�V�A�`����~�߂�

		Scenelist this_scene = Scenelist::scene_null; //����go�̂���scene

		std::list<std::shared_ptr<object>>::iterator go_iterator; //���g�ւ̃C�e���[�^�[(���g����?)

		//�A�^�b�`���ꂽ�R���|�[�l���g�̏���
		void initialize();
		void render();
		void update_P_to_C() {
			if (active == true)
			update();
			transform->local_orient = transform->local_orient.unit_vect();
			std::list<std::shared_ptr<object>>::iterator itr = children.begin();
			std::list<std::shared_ptr<object>>::iterator itr_end = children.end();
			for (; itr != itr_end;) {
				itr->get()->update_P_to_C();
				itr++;
			}
		}
		void update_world_trans() {
			transform->orientation = get_world_orientate();
			transform->position = get_world_position();
			transform->scale = get_world_scale();
		}
	private:


	public:
		//go��world��ԏ�ł̂̎p����Ԃ�
		quaternion get_world_orientate() {
			if (pearent != nullptr) return pearent->transform->orientation * transform->local_orient;
			else return transform->local_orient;
		};
		//go��world��ԏ�ł̍��W��Ԃ�
		vector3 get_world_position() {
			if (pearent != nullptr) return pearent->transform->position + transform->local_pos;
			else return transform->local_pos;
		};
		//go��world��ԏ�ł�scale��Ԃ�
		vector3 get_world_scale() {
			if (pearent != nullptr) return pearent->transform->scale * transform->local_scale;
			else return transform->local_scale;
		};



		void set_collider(Sphere* S, std::string tag, std::vector<std::string> no_hit_tag);
		void set_collider(Box* B,    std::string tag, std::vector<std::string> no_hit_tag);
		void set_collider(Plane* P,  std::string tag, std::vector<std::string> no_hit_tag);

		//:::::::::::::::::::::::::
		// pos : �`����W����̑��΍��W
		// r : ���a
		//:::::::::::::::::::::::::
		Sphere* add_collider_sphere(
			vector3 pos = vector3(0, 0, 0), 
			float r = 1, 
			float density = 1, 
			std::string tag = std::string("Sphere"), 
			std::vector<std::string> no_hit_tag = std::vector<std::string>()
		);

		//:::::::::::::::::::::::::
		// pos : �`����W����̑��΍��W
		// size : �`���]����� x,y,z �̑���half_size
		// rotate : �`���]����̑��Ή�]
		//:::::::::::::::::::::::::
		Box* add_collider_box(
			vector3 pos = vector3(0, 0, 0),
			vector3 scale = vector3(1, 1, 1), 
			quaternion orient = quaternion_identity(),
			float density = 1,
			std::string tag = std::string("Box"),
			std::vector<std::string> no_hit_tag = std::vector<std::string>()
		);

		//:::::::::::::::::::::::::
		// pos : ���΍��W
		// normal : ���Ζ@��
		//:::::::::::::::::::::::::
		Plane* add_collider_plane(
			vector3 pos = vector3(0, 0, 0),
			vector3 normal = vector3(0, 1, 0),
			float density = 1, 
			std::string tag = std::string("Plane"),
			std::vector<std::string> no_hit_tag = std::vector<std::string>()
		);


		//active���ύX���ꂽ�Ƃ��̏������Ăяo��
		void set_active(bool value) {
			if (active == value)return;
			active = value;
			if (value == false) {
				auto itr = components.begin();
				auto end = components.end();
				while (itr != end)
				{
					itr->get()->onDisable();
					itr++;
				}
			}
			else if (value == true) {
				auto itr = components.begin();
				auto end = components.end();
				while (itr != end)
				{
					itr->get()->onEnable();
					itr++;
				}
			}
		};
		// ==============================================================
		// ����GameObject�ɃR���|�[�l���g���A�^�b�`����
		// ==============================================================
		// typename T	:	�ǉ�����R���|�[�l���g��
		// ==============================================================
		// �߂�l T*		:	�ǉ������R���|�[�l���g��Ԃ��B���ɂ��邩�AComponent����h�����Ă��Ȃ��ꍇnullptr
		// ==============================================================
		template<typename T>
		T* addComponent()
		{
			// ���ɓ���R���|�[�l���g�����݂���ꍇ�AAdd���Ȃ�
			for (auto&& com : components)
			{
				//if (dynamic_cast<T*>(com) != nullptr)
				if (typeid(T) == typeid(*com))
				{
					//static_assert(0, "ERROR");
					return nullptr;
				}
			}

			//std::shared_ptr<T> newCom = std::make_shared<T>();
			T* newCom = new T();

			// Component�N���X����h���������̂��`�F�b�N
			Component* pCom = dynamic_cast<Component*>(newCom);
			if (pCom != nullptr)
			{
				components.emplace_back(std::shared_ptr<Component>(pCom));
				pCom->transform = this->transform;
				pCom->gameobject = this;
				pCom->input = MonoInput::getInstancePtr();
				pCom->time = Time::getInstancePtr();
				pCom->awake();		// �������������`���Ă���R���|�[�l���g�Ȃ�΂�����Ă�
				return newCom;
			}
			return nullptr;
		}

		// ==============================================================
		// ����GameObject�ɃA�^�b�`����Ă���R���|�[�l���g���폜����
		// ==============================================================
		// typename T	:	�폜�������R���|�[�l���g��
		// ==============================================================
		// �߂�l bool	:	�폜������true�A���Ă��Ȃ��Ȃ�false
		// ==============================================================
		template<typename T>
		bool removeComponent()
		{
			// ����R���|�[�l���g�����݂���ꍇ�Aremove����
			for (auto itr = components.begin(), itr_end = components.end(); itr != itr_end; ++itr)
			{
				if (typeid(T*) == typeid(*itr))
				{
					T* component = dynamic_cast<T*>(*itr);
					//delete component;
					component->finalize();
					components.erase(itr);
					return true;
				}
			}
			return false;
		}


		// ==============================================================
		// ����GameObject�ɃA�^�b�`����Ă���R���|�[�l���g�����ׂĊJ������
		// ==============================================================
		void clearComponent() {
			auto itr = components.begin();
			auto end = components.end();
			while (itr != end)
			{
				itr->get()->finalize();
				itr = components.erase(itr);
			}
			//components.clear();
		}


	};

}