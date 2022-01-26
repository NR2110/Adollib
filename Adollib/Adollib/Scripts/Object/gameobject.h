// ReSharper disable All
#pragma once

#include <string>
#include <list>
#include <memory>

#include "../Math/math.h"
#include "../Main/input.h"
#include "time.h"
//#include "Gameobject.h"
#include "component.h"
#include "transform.h"
#include "gameobject_tags.h"
#include "../Scene/scene_list.h"
#include "../Renderer/material.h"

namespace Adollib {
	class Time;
	class Renderer_base;

	class Gameobject {
	public:
		Gameobject(const bool l_no_material, const Scenelist l_this_scene, std::list<Gameobject*>::iterator l_this_itr) :
			this_scene(l_this_scene),
			this_itr(l_this_itr)
		{};

	private:
		void update();
		void update_worldtrans() {
			transform->orientation = world_orientate();
			transform->position = world_position();
			transform->scale = world_scale();
		}

		std::shared_ptr<Time>& get_time_ptr();

		std::list <Component*> components; //�A�^�b�`����Ă���Component�̃|�C���^

		Scenelist this_scene = Scenelist::scene_null; //����go�̂���scene

		std::list<Gameobject*>::iterator this_itr; //Gomanager�ɕۑ����ꂽ���g�ւ̃|�C���^

		Gameobject* parent_ = nullptr; //�e�ւ̃|�C���^
		std::list<Gameobject*> children_; //�q�ւ̃|�C���^

	public:
		// ����GO�����݂���V�[����Ԃ�
		Scenelist get_scene() const { return this_scene; };

		// ����GO�̃}�e���A���ւ̃|�C���^ cpp����y�ɃA�N�Z�X�ł���悤�ɂ����ɒu���Ă��� �����ɂȂ��Ă������Ɠ���
		//std::shared_ptr<Renderer> renderer = nullptr;
		Renderer_base* renderer = nullptr;

		std::shared_ptr<Transform> transform; //�s�{�ӂ�transform

		std::string name; //����go�̖��O

		bool is_active = true; //false�Ȃ�X�V�A�`����~�߂�

		bool is_hierarchy = true;

		u_int tag = GO_tag::None; //����go��tag(bit)


	public:
		// �A�^�b�`���ꂽ�R���|�[�l���g�̏���
		void initialize();

		void update_imgui_toChildren();

		//���g��update���Ă���q��update���Ă�
		void update_to_children() {
			if (is_active == true)
				update();
			//transform->local_orient = transform->local_orient.unit_vect();
			std::for_each(children_.begin(), children_.end(), [](Gameobject* obj) {obj->update_to_children(); });
		}

		//���g��update���Ă���q��update���Ă�
		void update_world_trans_to_children() {
			if (is_active == true)
				update_worldtrans();
			//transform->local_orient = transform->local_orient.unit_vect();
			std::for_each(children_.begin(), children_.end(), [](Gameobject* obj) {obj->update_world_trans_to_children(); });
		}

	public:
		// go��world��ԏ�ł̂̎p����Ԃ�
		const Quaternion world_orientate() const {
			if (parent() != nullptr) {
				return transform->local_orient * parent()->transform->orientation;
			}
			else return transform->local_orient;
		};
		// go��world��ԏ�ł̍��W��Ԃ�
		const Vector3 world_position() const {
			if (parent() != nullptr) {
				return parent()->transform->position + vector3_quatrotate(transform->local_pos * parent()->transform->scale, parent()->transform->orientation);
			}
			else return transform->local_pos;
		};
		// go��world��ԏ�ł�scale��Ԃ�
		const Vector3 world_scale() const {
			if (parent() != nullptr) {
				return parent()->transform->scale * transform->local_scale;
			}
			else return transform->local_scale;
		};

		// active���ύX���ꂽ�Ƃ��̏������Ăяo��
		void set_active(bool value) {
			if (is_active == value)return;
			is_active = value;
			if (value == false) {
				for (auto& comp : components)comp->onDisable();
			}
			else if (value == true) {
				for (auto& comp : components)comp->onEnable();

			}
		};

	public:

		Gameobject* parent() const { return parent_; }
		void set_parent(Gameobject* obj) { parent_ = obj; }
		const std::list<Gameobject*>* children() const { return &children_; }

		//��Ԃ̐e��Ԃ�
		Gameobject* top_parent() {
			if (parent_ != nullptr) return parent_->top_parent();
			return this;
		};

		//this�̎q�ɂ���
		void add_child(Gameobject* obj) {
			if (obj->parent() == this) return; //���łɂ���GO���e�Ƃ��ēo�^����Ă���
			if (obj->parent() != nullptr)obj->parent()->remove_child(obj);  //�O�̐e����폜
			obj->set_parent(this);
			children_.emplace_back(obj);
		};

		//�q����obj��remove����
		void remove_child(Gameobject* obj) {
			auto itr = children_.begin();
			auto itr_end = children_.end();
			for (; itr != itr_end; ++itr) {
				if (*itr == obj) {
					children_.erase(itr);
					break;
				}
			}
		};

	public:

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
				pCom->transform = this->transform;
				pCom->gameobject = this;
				pCom->input = MonoInput::getInstancePtr();
				pCom->time = get_time_ptr();
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
					delete component;
					components.erase(itr);
					return true;
				}
			}
			return false;
		}
		bool removeComponent(Component* comp)
		{
			// ����R���|�[�l���g�����݂���ꍇ�Aremove����
			for (auto itr = components.begin(), itr_end = components.end(); itr != itr_end; ++itr)
			{
				if (comp->com_itr == itr) {
					(*itr)->finalize();
					delete (*itr);
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