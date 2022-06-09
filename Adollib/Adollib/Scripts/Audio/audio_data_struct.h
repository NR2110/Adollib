#pragma once
#include <memory>

namespace DirectX
{
	class AudioEngine;
	class SoundEffect;
	class WaveBank;
	class SoundEffectInstance;
	class WAVData;
}

namespace Adollib {
	struct Audio_data {
		std::shared_ptr<DirectX::WAVData> wavInfo;
		std::unique_ptr<uint8_t[]> wavData;
		std::unique_ptr<DirectX::SoundEffect> soundEffect;
	};
}