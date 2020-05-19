#pragma once
#include "component.h"
#include "rigit_body.h"
#include "gameobject.h"

namespace Adollib
{
	class Rigitbody_manager : public Component
	{
	private:
		enum class Collidertype {
			Sphere,
			Box,
			Plane
		};
		struct RB_base {
			std::string tag;
			std::vector<std::string> No_hit_tag;
			Gameobject* for_drawing;
			Collidertype collider_type;
		};
		struct RB_sphere : public RB_base {
			Adollib::Sphere* R;
		};
		struct RB_box : public RB_base {
			Adollib::Box* R;
		};
		struct RB_plane : public RB_base {
			Adollib::Plane* R;
		};
		std::vector<RB_sphere> RB_sphere_s;
		std::vector<RB_box> RB_box_s;
		std::vector<RB_plane> RB_plane_s;

	public:
		void set_rigitbody(Adollib::Sphere* R, Gameobject* for_draw, std::string tag, std::vector<std::string> No_hit_tag = std::vector<std::string>());
		void set_rigitbody(Adollib::Box* R, Gameobject* for_draw, std::string tag, std::vector<std::string> No_hit_tag = std::vector<std::string>());
		void set_rigitbody(Adollib::Plane* R, Gameobject* for_draw, std::string tag, std::vector<std::string> No_hit_tag = std::vector<std::string>());

	public:
		void awake();

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		void start();

		// ���t���[���Ă΂��X�V����
		void update();

		// ���t���[���Aupdate()��ɌĂ΂��X�V����
		void lateUpdate();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
		void onEnable();

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		void onDisable();

	};

}