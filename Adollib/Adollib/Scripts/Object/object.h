//#pragma once
//#include <string>
//#include <memory>
//#include <list>
//#include "../Math/math.h"
//#include "transform.h"
//
//namespace Adollib
//{
//	class Gameobject;
//	// ::::::::::::::::::::
//	// GO�̊�{�N���X
//	// �G�ɍ�肷�������ߗp����
//	// :::::::::::::::::::::
//	class Object
//	{
//	private:
//		virtual void update() = 0;
//	public:
//		bool is_hierarchy = true;
//		bool active = true; //false�Ȃ�X�V�A�`����~�߂�
//		std::shared_ptr<Transform> transform; //�s�{�ӂ�transform
//
//	public:
//
//		virtual void initialize() {};
//		virtual void destroy() {};
//
//		virtual void update_worldtrans() {};
//		//virtual object* get_top_pearent() = 0;//��Ԃ̐e��Ԃ�
//
//		virtual const Quaternion world_orientate() const { return quaternion_identity(); };
//		virtual const Vector3 world_position() const { return Vector3(); };
//		virtual const Vector3 world_scale() const { return Vector3();	};
//
//	private:
//		Object* parent_ = nullptr; //�e�ւ̃|�C���^
//		std::list<Object*> children_; //�q�ւ̃|�C���^
//
//	public:
//		Object* parent() const{ return parent_;}
//		void set_parent(Object* obj) { parent_ = obj; }
//		const std::list<Object*>* children() const { return &children_; }
//
//		//��Ԃ̐e��Ԃ�
//		Object* top_parent() {
//			if (parent_ != nullptr) return parent_->top_parent();
//			return this;
//		};
//
//		//this�̎q�ɂ���
//		void add_child(Object* obj) {
//			if (obj->parent() == this) return; //���łɂ���GO���e�Ƃ��ēo�^����Ă���
//			if (obj->parent() != nullptr)obj->parent()->remove_child(obj);  //�O�̐e����폜
//			obj->set_parent(this);
//			children_.emplace_back(obj);
//		};
//
//		//�q����obj��remove����
//		void remove_child(Object* obj) {
//			auto itr = children_.begin();
//			auto itr_end = children_.end();
//			for (; itr != itr_end; ++itr) {
//				if (*itr == obj) {
//					children_.erase(itr);
//					break;
//				}
//			}
//		}
//
//		//virtual void update_P_to_C() {}; //�e����q��update���Ă�
//		virtual void update_imgui_toChildren() {};
//
//		//���g��update���Ă���q��update���Ă�
//		void update_to_children() {
//			if (active == true)
//				update();
//			//transform->local_orient = transform->local_orient.unit_vect();
//			std::for_each(children_.begin(), children_.end(), [](Object* obj) {obj->update_to_children(); });
//		}
//
//		//���g��update���Ă���q��update���Ă�
//		void update_world_trans_to_children() {
//			if (active == true)
//				update_worldtrans();
//			//transform->local_orient = transform->local_orient.unit_vect();
//			std::for_each(children_.begin(), children_.end(), [](Object* obj) {obj->update_world_trans_to_children(); });
//		}
//
//
//
//
//		Object() = default;
//		virtual ~Object()
//		{
//
//		};
//		Object(const Object&) = default;
//		void operator=(const Object&) = delete;
//
//		//Object(Object&&)noexcept = default;
//		Object& operator=(Object&&) = default;
//
//		//template<class Archive>
//		//void serialize(Archive& archive)
//		//{
//		//	archive(name);
//		//}
//
//		// ============================================
//		// �I�u�W�F�N�g�̃C���X�^���XID��Ԃ�(���z�֐�)
//		// ============================================
//		//virtual int getInstanceID() const = 0;
//
//		// ============================================
//		// GameObject����Ԃ�
//		// ============================================
//		//std::string toString() const
//		//{
//		//	return name;
//		//}
//
//	};
//
//
//
//}
//
//#pragma once