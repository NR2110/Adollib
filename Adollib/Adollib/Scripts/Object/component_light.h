#pragma once

#include "object.h"
#include "component.h"

//#include "../Shader/light_types.h"

#include "../Main/input.h"
#include "time.h"
#include <string>
#include <memory>
#include <vector>
#include <memory>

namespace Adollib
{
	struct POINTLIGHT;
	struct SPOTLIGHT;

	//�J�����p�̃R���|�[�l���g�N���X �p���s��!
	class Light_component final : public Component
	{
	public:
		std::vector <POINTLIGHT*> PointLight;
		std::vector <SPOTLIGHT*>  SpotLight;

		void set_dirLight(Vector3 dir, Vector3 color);

		void set_ambient(Vector3 amb);

		void set_pointLight(Vector3 pos, Vector3 color, float range);

		void set_spotLight(Vector3 pos, Vector3 color, Vector3 dir, float range, float near, float far);

	private:
		//manager�ɕۑ�����Ă��� ���g�ւ�itr
		std::list<Light_component*>::iterator this_itr;

	public:

		// addComponent���ꂽ�Ƃ��ɌĂ΂��
		void awake() override;

		// ��������V�[���̏��������Ɉ�x�����Ă΂��
		virtual void start() override {};

		// Hierarchy�̕\��(Imgui�̊֐� Imgui::begin,Imgui::end�͂���Ȃ�)
		virtual void Update_hierarchy() override;

		// ���t���[���Ă΂��X�V����
		virtual void update() override {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��(GO�������ɃX�N���v�g���L���Ȏ����Ă΂��)
		virtual void onEnable() override {};

		// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
		virtual void onDisable() override {};

		// removeComponent()�Aclear()���ɌĂ�
		void finalize() override;

	};



};
