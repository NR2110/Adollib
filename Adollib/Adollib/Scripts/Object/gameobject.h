#pragma once

#include <list>
#include <memory>

#include "../Main/input.h"
#include "time.h"
#include "object.h"
#include "component.h"
#include "transform.h"
#include "gameobject_tags.h"
#include "../Scene/scene_list.h"
#include "../Mesh/material.h"

namespace Adollib {

	class Gameobject : public object {
	public:
		Gameobject(bool l_no_material, Scenelist l_this_scene, std::list<Gameobject*>::iterator l_this_itr) :
			no_material(l_no_material),
			this_scene(l_this_scene),
			this_itr(l_this_itr)
		{};

	private:
		void update();
		void update_worldtrans() override {
			transform->orientation = world_orientate();
			transform->position = world_position();
			transform->scale = world_scale();
		}

		ComPtr<ID3D11Buffer> world_cb; //WVP�s��p�o�b�t�@

		bool no_material = false; //material�����������Ă��邩


		std::list <Component*> components; //�A�^�b�`����Ă���Conponent�̃|�C���^

		Scenelist this_scene = Scenelist::scene_null; //����go�̂���scene

		std::list<Gameobject*>::iterator this_itr; //���g�ւ̃C�e���[�^�[(���g����?)
	public:
		//����GO�����݂���V�[����Ԃ�
		Scenelist get_scene() { return this_scene; };

		u_int tag = GO_tag::None; //����go��tag(bit)

		std::string name; //����go�̖��O

		std::shared_ptr<Material> material; //����GO�̃}�e���A��


	public:

		//�A�^�b�`���ꂽ�R���|�[�l���g�̏���
		void initialize()override;

		void render()override;

		void update_imgui_toChildren() override;
	private:


	public:
		//go��world��ԏ�ł̂̎p����Ԃ�
		const Quaternion world_orientate() const override {
			if (pearent() != nullptr) {
				return transform->local_orient * pearent()->transform->orientation;
			}
			else return transform->local_orient;
		};
		//go��world��ԏ�ł̍��W��Ԃ�
		const Vector3 world_position() const override {
			if (pearent() != nullptr) {
				return pearent()->transform->position + vector3_quatrotate(transform->local_pos * pearent()->transform->scale, pearent()->transform->orientation);
			}
			else return transform->local_pos;
		};
		//go��world��ԏ�ł�scale��Ԃ�
		const Vector3 world_scale() const override {
			if (pearent() != nullptr) {
				return pearent()->transform->scale * transform->local_scale;
			}
			else return transform->local_scale;
		};

		//active���ύX���ꂽ�Ƃ��̏������Ăяo��
		void set_active(bool value) {
			if (active == value)return;
			active = value;
			if (value == false) {
				for (auto& comp : components)comp->onDisable();
			}
			else if (value == true) {
				for (auto& comp : components)comp->onEnable();

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
			// Component�N���X����h���������̂��`�F�b�N
			static_assert(std::is_base_of<Component, T>::value == true, "template T must inherit Component");

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
			T* newCom = newD T();

			Component* pCom = dynamic_cast<Component*>(newCom);
			if (pCom != nullptr)
			{
				components.emplace_back(pCom);
				std::list<Component*>::iterator itr = components.end();
				itr--;
				pCom->com_itr = itr;
				pCom->transform = this->transform.get();
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
			//component�̏I���������s��
			for (auto& comp : components)
			{
				comp->finalize();
				delete comp;
			}
			components.clear();
		}

		//void removeGO() {

		//}

		//�������
		void destroy();
	};

}