// ReSharper disable All
#pragma once

#include <list>
#include <memory>

#include "../Main/input.h"
#include "time.h"
#include "Object.h"
#include "component.h"
#include "transform.h"
#include "gameobject_tags.h"
#include "../Scene/scene_list.h"
#include "../Renderer/material.h"

namespace Adollib {

	class Gameobject : public Object {
	public:
		Gameobject(const bool l_no_material, const Scenelist l_this_scene, std::list<Gameobject*>::iterator l_this_itr) :
			this_scene(l_this_scene),
			this_itr(l_this_itr)
		{};

	private:
		void update() override;
		void update_worldtrans() override {
			transform->orientation = world_orientate();
			transform->position = world_position();
			transform->scale = world_scale();
		}

		std::list <Component*> components; //�A�^�b�`����Ă���Component�̃|�C���^

		Scenelist this_scene = Scenelist::scene_null; //����go�̂���scene

		std::list<Gameobject*>::iterator this_itr; //Gomanager�ɕۑ����ꂽ���g�ւ̃|�C���^

	public:
		// ����GO�����݂���V�[����Ԃ�
		Scenelist get_scene() const { return this_scene; };

		u_int tag = GO_tag::None; //����go��tag(bit)

		std::string name; //����go�̖��O

		// ����GO�̃}�e���A���ւ̃|�C���^ cpp����y�ɃA�N�Z�X�ł���悤�ɂ����ɒu���Ă��� �����ɂȂ��Ă������Ɠ���
		std::shared_ptr<Material> material = nullptr;


	public:

		// �A�^�b�`���ꂽ�R���|�[�l���g�̏���
		void initialize()override;

		void update_imgui_toChildren() override;
	private:


	public:
		// go��world��ԏ�ł̂̎p����Ԃ�
		const Quaternion world_orientate() const override {
			if (parent() != nullptr) {
				return transform->local_orient * parent()->transform->orientation;
			}
			else return transform->local_orient;
		};
		// go��world��ԏ�ł̍��W��Ԃ�
		const Vector3 world_position() const override {
			if (parent() != nullptr) {
				return parent()->transform->position + vector3_quatrotate(transform->local_pos * parent()->transform->scale, parent()->transform->orientation);
			}
			else return transform->local_pos;
		};
		// go��world��ԏ�ł�scale��Ԃ�
		const Vector3 world_scale() const override {
			if (parent() != nullptr) {
				return parent()->transform->scale * transform->local_scale;
			}
			else return transform->local_scale;
		};

		// active���ύX���ꂽ�Ƃ��̏������Ăяo��
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


		// ����GameObject�ɃR���|�[�l���g���A�^�b�`����
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

		template<typename T>
		T* findComponent(){
			// Component�N���X����h���������̂��`�F�b�N
			static_assert(std::is_base_of<Component, T>::value == true, "template T must inherit Component");

			for (auto com : components) {
				T* comp = dynamic_cast<T*>(com) ;
				if (comp != nullptr) return comp;
			}

			return nullptr;
		}


		// ����GameObject�ɃA�^�b�`����Ă���R���|�[�l���g���폜����
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


		// ����GameObject�ɃA�^�b�`����Ă���R���|�[�l���g�����ׂĊJ������
		void clearComponent() {
			//component�̏I���������s��
			for (auto& comp : components)
			{
				comp->finalize();
				delete comp;
			}
			components.clear();
		}

		// �������
		void destroy();
	};

}