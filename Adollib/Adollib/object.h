#pragma once
#include <string>

namespace Adollib
{
	// *************************************************
	// �S�ẴI�u�W�F�N�g�̊�{�N���X
	// *************************************************
	class object
	{
	public:
		//protected:
		//std::string name;	// �I�u�W�F�N�g��

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