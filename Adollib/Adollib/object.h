#pragma once
#include <string>
#include <memory>
#include <list>
#include "quaternion.h"
#include "transform.h"

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

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
		struct collider_effect {
			vector3 position = vector3(0,0,0);
			quaternion orient = quaternion_identity();
		};
		virtual void update() = 0;
	public:
		//protected:
		//std::string name;	// オブジェクト名

		bool updated; 
		std::shared_ptr<Transfome> transform; //不本意なtransform
		collider_effect co_e; //衝突計算から受ける処理の一時的保存場所

	public:

		virtual void initialize() {};
		virtual void render() {};
		virtual void destroy() {};

		virtual void update_P_to_C() {}; //親から子へupdateを呼ぶ
		virtual void update_world_trans() {};
		virtual object* get_pearent() = 0;//一番の親を返す
		virtual	std::list<std::shared_ptr<object>> get_children() = 0;	//すべての子を返す

		virtual quaternion get_world_orientate() { return quaternion_identity(); };
		virtual vector3 get_world_position() { return vector3(); };
		virtual vector3 get_world_scale() { return vector3();	};


	

		object() = default;
		virtual ~object()
		{

		}
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