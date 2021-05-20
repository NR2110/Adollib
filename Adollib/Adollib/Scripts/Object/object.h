#pragma once
#include <string>
#include <memory>
#include <list>
#include "../Math/math.h"
#include "transform.h"

namespace Adollib
{
	class Gameobject;
	// ::::::::::::::::::::
	// GO�̊�{�N���X
	// �G�ɍ�肷�������ߗp����
	// :::::::::::::::::::::
	class object
	{
	private:
		virtual void update() = 0;
	public:
		bool is_hierarchy = true;
		bool active = true; //false�Ȃ�X�V�A�`����~�߂�
		std::shared_ptr<Transfome> transform; //�s�{�ӂ�transform

	public:

		virtual void initialize() {};
		virtual void render() {};
		virtual void destroy() {};

		virtual void update_worldtrans() {};
		//virtual object* get_top_pearent() = 0;//��Ԃ̐e��Ԃ�

		virtual Quaternion world_orientate() { return quaternion_identity(); };
		virtual Vector3 world_position() { return Vector3(); };
		virtual Vector3 world_scale() { return Vector3();	};

	private:
		object* pearent_ = nullptr; //�e�ւ̃|�C���^
		std::list<object*> children_; //�q�ւ̃|�C���^

	public:
		object* pearent() const{ return pearent_;}
		void set_pearent(object* obj) { pearent_ = obj; }
		std::list<object*>* children() { return &children_; }

		//��Ԃ̐e��Ԃ�
		object* top_pearent() {
			if (pearent_ != nullptr) return pearent_->top_pearent();
			return this;
		};

		//this�̎q�ɂ���
		void add_child(object* obj) {
			if (obj->pearent() == this) return; //���łɂ���GO���e�Ƃ��ēo�^����Ă���
			if (obj->pearent() != nullptr)obj->pearent()->remove_child(obj);  //�O�̐e����폜
			obj->set_pearent(this);
			children_.emplace_back(obj);
		};

		//�q����obj��remove����
		void remove_child(object* obj) {
			auto itr = children_.begin();
			auto itr_end = children_.end();
			for (; itr != itr_end; ++itr) {
				if (*itr == obj) {
					children_.erase(itr);
					break;
				}
			}
		}

		//virtual void update_P_to_C() {}; //�e����q��update���Ă�
		virtual void update_imgui_toChildren() {};

		//���g��update���Ă���q��update���Ă�
		void update_to_children() {
			if (active == true)
				update();
			//transform->local_orient = transform->local_orient.unit_vect();
			std::for_each(children_.begin(), children_.end(), [](object* obj) {obj->update_to_children(); });
		}

		//���g��update���Ă���q��update���Ă�
		void update_world_trans_to_children() {
			if (active == true)
				update_worldtrans();
			//transform->local_orient = transform->local_orient.unit_vect();
			std::for_each(children_.begin(), children_.end(), [](object* obj) {obj->update_world_trans_to_children(); });
		}




		object() = default;
		virtual ~object()
		{

		};
		object(const object&) = default;
		void operator=(const object&) = delete;

		//Object(Object&&)noexcept = default;
		object& operator=(object&&) = default;

		//template<class Archive>
		//void serialize(Archive& archive)
		//{
		//	archive(name);
		//}

		// ============================================
		// �I�u�W�F�N�g�̃C���X�^���XID��Ԃ�(���z�֐�)
		// ============================================
		//virtual int getInstanceID() const = 0;

		// ============================================
		// GameObject����Ԃ�
		// ============================================
		//std::string toString() const
		//{
		//	return name;
		//}

	};



}

#pragma once