#pragma once

#include "../Math/math.h"

namespace Adollib {
	struct Instance
	{
		DirectX::XMFLOAT4X4 transformMatrix = {};	// world�ϊ���NDC�ϊ�������s��
		DirectX::XMFLOAT4 texcoordTransform = { 0.0f,0.0f,1.0f,1.0f };
		DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
	};

	// �`��X�^�[�g�A�J�E���g���܂Ƃ߂��\����
	class Renderer_base;
	struct Render_count
	{
		int bufferStart = 0;			// �C���X�^���X�o�b�t�@���̕`��J�n�ʒu
		int bufferCount = 1;			// �C���X�^���X�o�b�t�@���̃X�^�[�g����̕`�搔
		Renderer_base* renderer = nullptr;
	};
}