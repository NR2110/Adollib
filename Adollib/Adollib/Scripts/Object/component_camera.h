#pragma once

#include "Object.h"
#include "component.h"
#include "../Main/input.h"
#include "time.h"
#include <string>
#include <memory>

#include <d3d11.h>
#include <wrl.h>

namespace Adollib
{
	//�J�����p�̃R���|�[�l���g�N���X �p���s��!
	class Camera_component final : public Component
	{
	public:
		float fov = 60.0f;
		float aspect = 1280 / 720.0f;
		float nearZ = 0.1f;
		float farZ = 1000000.0f;


	private:
		// manager�ɕۑ�����Ă��� ���g�ւ�itr
		std::list<Camera_component*>::iterator this_itr;

		Microsoft::WRL::ComPtr<ID3D11Buffer> view_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> projection_cb;

	public:
		// ConstantBuffer��camera����set����
		void set_Constantbuffer();

	public:

		// addComponent���ꂽ�Ƃ��ɌĂ΂��
		void awake() override;

		// Hierarchy�̕\��(Imgui�̊֐� Imgui::begin,Imgui::end�͂���Ȃ�)
		virtual void Update_hierarchy() override;

		// removeComponent()�Aclear()���ɌĂ�
		void finalize() override;

	};



};
