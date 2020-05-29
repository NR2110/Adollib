#pragma once
#include <string>

namespace Adollib
{
	// *************************************************
	// 全てのオブジェクトの基本クラス
	// *************************************************
	class object
	{
	public:
		//protected:
		//std::string name;	// オブジェクト名

		bool updated;
	public:
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