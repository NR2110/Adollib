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
//	// GOの基本クラス
//	// 雑に作りすぎたため用調整
//	// :::::::::::::::::::::
//	class Object
//	{
//	private:
//		virtual void update() = 0;
//	public:
//		bool is_hierarchy = true;
//		bool active = true; //falseなら更新、描画を止める
//		std::shared_ptr<Transform> transform; //不本意なtransform
//
//	public:
//
//		virtual void initialize() {};
//		virtual void destroy() {};
//
//		virtual void update_worldtrans() {};
//		//virtual object* get_top_pearent() = 0;//一番の親を返す
//
//		virtual const Quaternion world_orientate() const { return quaternion_identity(); };
//		virtual const Vector3 world_position() const { return Vector3(); };
//		virtual const Vector3 world_scale() const { return Vector3();	};
//
//	private:
//		Object* parent_ = nullptr; //親へのポインタ
//		std::list<Object*> children_; //子へのポインタ
//
//	public:
//		Object* parent() const{ return parent_;}
//		void set_parent(Object* obj) { parent_ = obj; }
//		const std::list<Object*>* children() const { return &children_; }
//
//		//一番の親を返す
//		Object* top_parent() {
//			if (parent_ != nullptr) return parent_->top_parent();
//			return this;
//		};
//
//		//thisの子にする
//		void add_child(Object* obj) {
//			if (obj->parent() == this) return; //すでにこのGOが親として登録されていた
//			if (obj->parent() != nullptr)obj->parent()->remove_child(obj);  //前の親から削除
//			obj->set_parent(this);
//			children_.emplace_back(obj);
//		};
//
//		//子からobjをremoveする
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
//		//virtual void update_P_to_C() {}; //親から子へupdateを呼ぶ
//		virtual void update_imgui_toChildren() {};
//
//		//自身のupdateしてから子のupdateを呼ぶ
//		void update_to_children() {
//			if (active == true)
//				update();
//			//transform->local_orient = transform->local_orient.unit_vect();
//			std::for_each(children_.begin(), children_.end(), [](Object* obj) {obj->update_to_children(); });
//		}
//
//		//自身のupdateしてから子のupdateを呼ぶ
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
//		// オブジェクトのインスタンスIDを返す(仮想関数)
//		// ============================================
//		//virtual int getInstanceID() const = 0;
//
//		// ============================================
//		// GameObject名を返す
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