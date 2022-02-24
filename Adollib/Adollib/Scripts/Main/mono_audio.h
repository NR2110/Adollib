#pragma once
#include "Audio.h"
#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>
#include <stdio.h>

namespace DirectX
{
	class AudioEngine;
	class SoundEffect;
	class WaveBank;
	class SoundEffectInstance;

}

namespace ALKLib
{
	static constexpr int MUSIC_FILE_MAX = 32;
	static constexpr int WAVE_FILE_MAX = 128;
	static constexpr int WAVE_SOUND_MAX = 8;   // 最大8音同時再生OK

	enum SoundState
	{
		STOPPED = 0,
		PLAYING,
		PAUSED
	};
	enum Music
	{
		BGM_Title,
		BGM_Game,

		SE_Pop = 10,
		SE_Jump,
		SE_Landing,
		SE_Step0,
		SE_Step1,
		SE_Button_on,
		SE_Button_off,

	};
	enum class Sound
	{
		SE_ = 0,

	};
	// ===================================================
	// MonoAudio
	// ===================================================
	class MonoAudio
	{
	private:
		// AudioEnngine
		static std::unique_ptr<DirectX::AudioEngine>			audioEngine;

		// Music
		struct MusicData
		{
			std::unique_ptr<DirectX::SoundEffect>			music[MUSIC_FILE_MAX];
			std::unique_ptr<DirectX::SoundEffectInstance>	musicInst[MUSIC_FILE_MAX];
			float											musicVolume[MUSIC_FILE_MAX] = {};
		} static musicData;

		// Sound
		struct SoundData
		{
			std::unique_ptr<DirectX::WaveBank>				waveBank;
			std::unique_ptr<DirectX::SoundEffectInstance>	soundInst[WAVE_FILE_MAX][WAVE_SOUND_MAX];	// 最大8音を同時再生可能
			float											soundVolume[WAVE_FILE_MAX] = {};
		} static soundData;

	public:
		static void awake();
		static void destroy();

		static void Init();

		//  オーディオエンジンのリセット
		static bool Reset();

		//----------------------------------------------------
		//		BGM
		//----------------------------------------------------
		// 読込
		static void LoadMusic(int trackNo, const wchar_t* waveFileName, float volume = (0.5f));
		//  解放
		static void UnloadMusic(int trackNo);

		//  再生
		static void PlayMusic(int trackNo, bool isLoop = (false));
		//  停止
		static void StopMusic(int trackNo);
		//  一時停止
		static void PauseMusic(int trackNo);
		//  再開（一時停止からの）
		static void ResumeMusic(int trackNo);

		//  ボリューム設定
		static void SetMusicVolume(int trackNo, float volume);
		//  panの設定
		static void SetMusicPan(int trackNo, float pan);
		//  pitchの設定
		static void SetMusicPitch(int trackNo, float pitch);

		//  SoundStateの取得(停止、再生中、一時停止の取得)
		static SoundState GetMusicState(int trackNo);
		//  ループの有無の取得
		static bool IsMusicLooped(int trackNo);
		//  使用中かどうか
		static bool IsInUseMusic(int trackNo);
		//  フォーマットの取得
		static const WAVEFORMATEX* GetMusicFormat(int trackNo);

		//----------------------------------------------------
		//		SE
		//----------------------------------------------------
		//  読込
		static void LoadSound(const wchar_t* xwbFileName, float volume = (0.5f));

		//  再生
		static void PlaySoundE(int trackNo, float pitch = 0.f, float pan = 0.f, bool looping = false);
		//  停止
		static void StopSound(int trackNo);
		//  一時停止
		static void PauseSound(int trackNo);
		//  一時再開(一時停止から)
		static void ResumeSound(int trackNo);

		//  ボリューム設定
		static void SetSoundVolume(int trackNo, float volume);

		//  指定したトラックのいずれかのループ状態を取得
		static bool IsSoundLooped(int trackNo);
		static bool IsAudioDevicePresent() { return audioEngine->IsAudioDevicePresent(); }
		static bool IsCriticalError() { return audioEngine->IsCriticalError(); }

	};
}