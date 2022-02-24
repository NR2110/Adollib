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
	static constexpr int WAVE_SOUND_MAX = 8;   // �ő�8�������Đ�OK

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
			std::unique_ptr<DirectX::SoundEffectInstance>	soundInst[WAVE_FILE_MAX][WAVE_SOUND_MAX];	// �ő�8���𓯎��Đ��\
			float											soundVolume[WAVE_FILE_MAX] = {};
		} static soundData;

	public:
		static void awake();
		static void destroy();

		static void Init();

		//  �I�[�f�B�I�G���W���̃��Z�b�g
		static bool Reset();

		//----------------------------------------------------
		//		BGM
		//----------------------------------------------------
		// �Ǎ�
		static void LoadMusic(int trackNo, const wchar_t* waveFileName, float volume = (0.5f));
		//  ���
		static void UnloadMusic(int trackNo);

		//  �Đ�
		static void PlayMusic(int trackNo, bool isLoop = (false));
		//  ��~
		static void StopMusic(int trackNo);
		//  �ꎞ��~
		static void PauseMusic(int trackNo);
		//  �ĊJ�i�ꎞ��~����́j
		static void ResumeMusic(int trackNo);

		//  �{�����[���ݒ�
		static void SetMusicVolume(int trackNo, float volume);
		//  pan�̐ݒ�
		static void SetMusicPan(int trackNo, float pan);
		//  pitch�̐ݒ�
		static void SetMusicPitch(int trackNo, float pitch);

		//  SoundState�̎擾(��~�A�Đ����A�ꎞ��~�̎擾)
		static SoundState GetMusicState(int trackNo);
		//  ���[�v�̗L���̎擾
		static bool IsMusicLooped(int trackNo);
		//  �g�p�����ǂ���
		static bool IsInUseMusic(int trackNo);
		//  �t�H�[�}�b�g�̎擾
		static const WAVEFORMATEX* GetMusicFormat(int trackNo);

		//----------------------------------------------------
		//		SE
		//----------------------------------------------------
		//  �Ǎ�
		static void LoadSound(const wchar_t* xwbFileName, float volume = (0.5f));

		//  �Đ�
		static void PlaySoundE(int trackNo, float pitch = 0.f, float pan = 0.f, bool looping = false);
		//  ��~
		static void StopSound(int trackNo);
		//  �ꎞ��~
		static void PauseSound(int trackNo);
		//  �ꎞ�ĊJ(�ꎞ��~����)
		static void ResumeSound(int trackNo);

		//  �{�����[���ݒ�
		static void SetSoundVolume(int trackNo, float volume);

		//  �w�肵���g���b�N�̂����ꂩ�̃��[�v��Ԃ��擾
		static bool IsSoundLooped(int trackNo);
		static bool IsAudioDevicePresent() { return audioEngine->IsAudioDevicePresent(); }
		static bool IsCriticalError() { return audioEngine->IsCriticalError(); }

	};
}