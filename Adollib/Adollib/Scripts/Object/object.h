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
	// GOの基本クラス
	// 雑に作りすぎたため用調整
	// :::::::::::::::::::::
	class object
	{
	private:
		virtual void update() = 0;
	public:
		bool active = true; //falseなら更新、描画を止める
		std::shared_ptr<Transfome> transform; //不本意なtransform

	public:

		virtual void initialize() {};
		virtual void render() {};
		virtual void destroy() {};

		//virtual void update_P_to_C() {}; //親から子へupdateを呼ぶ
		virtual void update_imgui_P_to_C() {};
		virtual void update_world_trans() {};
		//virtual object* get_top_pearent() = 0;//一番の親を返す

		virtual Quaternion get_world_orientate() { return quaternion_identity(); };
		virtual Vector3 get_world_position() { return Vector3(); };
		virtual Vector3 get_world_scale() { return Vector3();	};

	private:
		object* pearent_ = nullptr; //親へのポインタ
		std::list<object*> children_; //子へのポインタ
	public:
		object* pearent() const{ return pearent_;}
		void pearent(object* obj) { pearent_ = obj; }
		const std::list<object*>* children() { return &children_; }

		//一番の親を返す
		object* top_pearent() {
			if (pearent_ != nullptr) return pearent_->top_pearent();
			return this;
		};

		//thisの子にする
		void add_child(object* obj) {
			if (obj->pearent() == this) return; //すでにこのGOが親として登録されていた
			if (obj->pearent() != nullptr)obj->pearent()->remove_child(this);  //前の親から削除
			obj->pearent(this);
			children_.emplace_back(obj);
		};

		//子からobjをremoveする
		void remove_child(object* obj) {
			auto itr = children_.begin();
			auto itr_end = children_.end();
			for (; itr != itr_end; ++itr) {
				if (*itr == obj)
					children_.erase(itr);
			}
		}

		//自身のupdateしてから子のupdateを呼ぶ
		void update_P_to_C() {
			if (active == true)
				update();
			transform->local_orient = transform->local_orient.unit_vect();
			std::for_each(children_.begin(), children_.end(), [](object* obj) {obj->update_P_to_C(); });
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
		// オブジェクトのインスタンスIDを返す(仮想関数)
		// ============================================
		//virtual int getInstanceID() const = 0;

		// ============================================
		// GameObject名を返す
		// ============================================
		//std::string toString() const
		//{
		//	return name;
		//}

	};



}

#pragma once