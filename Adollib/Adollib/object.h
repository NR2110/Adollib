#pragma once
#include <string>
#include <memory>
#include <list>
#include "quaternion.h"
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
		struct collider_effect {
			vector3 position;
			quaternion orient;
		};
		virtual void update() = 0;
	public:
		//protected:
		//std::string name;	// �I�u�W�F�N�g��

		bool updated; 
		Transfome* transform; //�s�{�ӂ�transform
		collider_effect co_e; //�Փˌv�Z����󂯂鏈���̈ꎞ�I�ۑ��ꏊ

	public:

		virtual void initialize() {};
		virtual void render() {};

		virtual void update_P_to_C() {}; //�e����q��update���Ă�
		virtual void update_world_trans() {};
		virtual object* get_pearent() = 0;//��Ԃ̐e��Ԃ�
		virtual	std::list<std::shared_ptr<object>> get_children() = 0;	//���ׂĂ̎q��Ԃ�

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