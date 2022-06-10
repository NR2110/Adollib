#pragma once

//

#include <unordered_map>
#include "audio_data_struct.h"
#include "../../DirectXTK-master/Inc/Audio.h"


namespace Adollib {
	class Audio_listener;

	static class Audio_resource_manager {


	private:
		// load����audio�̃��\�[�X
		static std::unordered_map<std::string, Audio_data> audio_datas;

	public:
		// file��load
		static Audio_data* load(DirectX::AudioEngine* audioEngine, const std::string& file_path);
		// ���\�[�X�̉��
		static void release(const std::string& file_path);


	public:

	};
}