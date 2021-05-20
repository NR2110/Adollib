#pragma once

#include <list>
#include <memory>

#include "../Main/input.h"
#include "time.h"
#include "gameobject.h"
#include "component_camera.h"
#include "transform.h"
#include "../Scene/scene_list.h"

namespace Adollib {

	class Camera : public object {
	public:
		Camera(Scenelist l_this_scene, std::list<Camera*>::iterator l_this_itr) :
			this_scene(l_this_scene),
			this_itr(l_this_itr)
		{};

	private:
		void update();

		std::list<Camera*>::iterator this_itr; //���g�ւ̃C�e���[�^�[(���g����?)

		std::list <Component_camera*> components; //�A�^�b�`����Ă���Conponent�̃|�C���^

		Scenelist this_scene = Scenelist::scene_null; //����go�̂���scene
	public:
		std::string name = std::string("null"); //����go�̖��O(�����p)


		float fov = 60.0f;
		float aspect = 1280 / 720.0f;
		float nearZ = 0.1f;
		float farZ = 1000000.0f;

		void update_imgui_toChildren() override;

		void update_worldtrans() override {
			transform->orientation = get_world_orientate();
			transform->position = get_world_position();
			transform->scale = get_world_scale();
		}


		//�A�^�b�`���ꂽ�R���|�[�l���g�̏���
		void initialize();
		void render() {};

	private:

	public:
		Quaternion get_world_orientate() {
			if (pearent() != nullptr) return pearent()->transform->orientation * transform->local_orient;
			else return transform->local_orient;
		};
		Vector3 get_world_position() {
			if (pearent() != nullptr) return pearent()->transform->position + transform->local_pos;
			else return transform->local_pos;
		};
		Vector3 get_world_scale() {
			if (pearent() != nullptr) return pearent()->transform->scale * transform->local_scale;
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
			T* newCom = DBG_NEW T();

			// Component�N���X����h���������̂��`�F�b�N
			Component_camera* pCom = dynamic_cast<Component_camera*>(newCom);
			if (pCom != nullptr)
			{
				components.emplace_back(pCom);
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

		//�������
		void destroy();
	};

}