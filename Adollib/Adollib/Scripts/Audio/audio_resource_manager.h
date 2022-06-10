#pragma once

//

#include <unordered_map>
#include "audio_data_struct.h"
#include "../../DirectXTK-master/Inc/Audio.h"


namespace Adollib {
	class Audio_listener;

	static class Audio_resource_manager {


	private:
		// loadしたaudioのリソース
		static std::unordered_map<std::string, Audio_data> audio_datas;

	public:
		// fileのload
		static Audio_data* load(DirectX::AudioEngine* audioEngine, const std::string& file_path);
		// リソースの解放
		static void release(const std::string& file_path);


	public:

	};
}