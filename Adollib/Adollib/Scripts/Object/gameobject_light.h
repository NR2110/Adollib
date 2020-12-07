#pragma once

#include <list>
#include <memory>

#include "../Main/input.h"
#include "time.h"
#include "gameobject.h"
#include "component_light.h"
#include "transform.h"
#include "../Scene/scene_list.h"
#include "../Shader/light_types.h"

namespace Adollib {

	class Light : public object {
	private:
		void update();
	public:
		std::string name = std::string("null"); //����go�̖��O(�����p)

		std::list <std::shared_ptr<Component_light>> components; //�A�^�b�`����Ă���Conponent�̃|�C���^

		Scenelist this_scene = Scenelist::scene_null; //����go�̂���scene

		std::list<std::shared_ptr<Light>>::iterator go_iterator; //���g�ւ̃C�e���[�^�[(���g����?)B

		void update_imgui_P_to_C() override;

		void update_world_trans() {
			//transform->orientation = get_world_orientate();
			//transform->position = get_world_position();
			//transform->scale = get_world_scale();
		}

		std::vector <std::shared_ptr<POINTLIGHT>> PointLight;
		std::vector <std::shared_ptr<SPOTLIGHT>>  SpotLight;

		void set_dirLight(Vector3 dir, Vector3 color);

		void set_ambient(Vector3 amb);

		void set_pointLight(Vector3 pos, Vector3 color, float range);

		void set_spotLight(Vector3 pos, Vector3 color, Vector3 dir, float range, float near, float far);

		//�A�^�b�`���ꂽ�R���|�[�l���g�̏���
		void initialize();
		void render() {};
	private:

	public:
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
			T* newCom = DBG_NEW T();

			// Component�N���X����h���������̂��`�F�b�N
			Component_light* pCom = dynamic_cast<Component_light*>(newCom);
			if (pCom != nullptr)
			{
				components.emplace_back(std::shared_ptr<Component_light>(pCom));
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
				if (itr->get()) delete itr->get();
				itr = components.erase(itr);
			}
			//components.clear();
		}

		//�������
		void destroy() {
			clearComponent();
		}
	};

}