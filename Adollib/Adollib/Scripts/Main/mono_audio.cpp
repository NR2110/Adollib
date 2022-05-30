#include <assert.h>
#include "mono_audio.h"

using namespace DirectX;
using namespace std;

namespace Adollib
{
	using DirectX::AUDIO_ENGINE_FLAGS;
	using DirectX::AudioEngine;
	using DirectX::SoundEffect;
	using DirectX::WaveBank;

	using DirectX::AudioEngine_Default;
	using DirectX::AudioEngine_Debug;

	// AudioEnngine
	std::unique_ptr<DirectX::AudioEngine> MonoAudio::audioEngine;
	AudioListener MonoAudio::listener; //自身の座標
	AudioEmitter MonoAudio::emitter;   //音の座標

	// Music
	MonoAudio::MusicData MonoAudio::musicData;

	// Sound
	MonoAudio::SoundData MonoAudio::soundData;

	void MonoAudio::Init() {

		// BGMのロード
		LoadMusic(static_cast<int>(Music::BGM_Title),  L"../Data/sounds/BGM/Take_it_easy.wav", 0.25f);
		//LoadMusic(static_cast<int>(Music::BGM_Title),  L"../Data/sounds/BGM/otoboke.wav", 0.25f);
		LoadMusic(static_cast<int>(Music::BGM_Game), L"../Data/sounds/BGM/tuitekuru_tuitekuru.wav", 0.6f);
		//LoadMusic(static_cast<int>(Music::BGM_Title), L"../Data/sounds/BGM/Take_it_easy.wav", 0);
		//LoadMusic(static_cast<int>(Music::BGM_Game), L"../Data/sounds/BGM/tuitekuru_tuitekuru.wav", 0);
		LoadMusic(static_cast<int>(Music::DEMO), L"../Data/sounds/Demo_BGM/Theme_of_Rabi-Ribi.wav", 0.15f);

		LoadMusic(static_cast<int>(Music::SE_Pop),  L"../Data/sounds/SE/Motion-Pop03-2.wav", 0.5f);
		LoadMusic(static_cast<int>(Music::SE_Jump), L"../Data/sounds/SE/SNES-Action01-01(Jump).wav", 0.5f);
		LoadMusic(static_cast<int>(Music::SE_Landing), L"../Data/sounds/SE/SNES-Action01-03(Stomp).wav", 0.7f);
		LoadMusic(static_cast<int>(Music::SE_Step0), L"../Data/sounds/SE/Motion-Pop28-1.wav", 0.2f);
		LoadMusic(static_cast<int>(Music::SE_Step1), L"../Data/sounds/SE/Motion-Pop28-3.wav", 0.2f);
		LoadMusic(static_cast<int>(Music::SE_Button_on), L"../Data/sounds/SE/Onmtp-Click02-1.wav", 0.15f);
		LoadMusic(static_cast<int>(Music::SE_Button_off), L"../Data/sounds/SE/Onmtp-Click02-1.wav", 0.15f);

		LoadMusic(static_cast<int>(Music::SE_Pop       ) + 10, L"../Data/sounds/SE/Motion-Pop03-2.wav", 0.5f);
		LoadMusic(static_cast<int>(Music::SE_Jump      ) + 10, L"../Data/sounds/SE/SNES-Action01-01(Jump).wav", 0.5f);
		LoadMusic(static_cast<int>(Music::SE_Landing   ) + 10, L"../Data/sounds/SE/SNES-Action01-03(Stomp).wav", 0.7f);
		LoadMusic(static_cast<int>(Music::SE_Step0     ) + 10, L"../Data/sounds/SE/Motion-Pop28-1.wav", 0.2f);
		LoadMusic(static_cast<int>(Music::SE_Step1     ) + 10, L"../Data/sounds/SE/Motion-Pop28-3.wav", 0.2f);
		LoadMusic(static_cast<int>(Music::SE_Button_on ) + 10, L"../Data/sounds/SE/Onmtp-Click02-1.wav", 0.15f);
		LoadMusic(static_cast<int>(Music::SE_Button_off) + 10, L"../Data/sounds/SE/Onmtp-Click02-1.wav", 0.15f);



		//// BGMのロード
		//LoadSound(L"Assets/Sounds/SE/Win/sound_effect.xwb");

	}

	//--------------------------------
	//  コンストラクタ
	//--------------------------------
	void MonoAudio::awake()
	{
#if 0
		{
			// This is only needed in Win32 desktop apps
			HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			if (FAILED(hr))assert("Audio, CoInitializeEx failed");
		}
#endif

		// Create DirectXTK for Audio objects
		//DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;
		DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_ReverbUseFilters | DirectX::AudioEngine_EnvironmentalReverb;
#ifdef _DEBUG
		eflags = eflags | DirectX::AudioEngine_Debug;
#endif
		audioEngine = make_unique<AudioEngine>(eflags);
		audioEngine->SetReverb(Reverb_Default);

		for (auto& p : musicData.musicVolume) p = 1.0f;
		for (auto& p : soundData.soundVolume) p = 1.0f;

	}

	//--------------------------------
	//  デストラクタ
	//--------------------------------
	void MonoAudio::destroy()
	{
		audioEngine.reset();
	}

	//--------------------------------
	//  オーディオエンジンのリセット
	//--------------------------------
	bool MonoAudio::Reset()
	{
		return true;

		//return audioEngine->Reset();
	}

	//--------------------------------
	//  音楽読込
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	//  const wchar_t* waveFileName     音楽ファイルのパス
	//  float volume                    ボリューム (0.0f ~ 1.0f)
	void MonoAudio::LoadMusic(int trackNo, const wchar_t* waveFileName, float volume)
	{
		if (musicData.musicInst[trackNo]) musicData.musicInst[trackNo].reset();
		musicData.music[trackNo].reset(new SoundEffect(audioEngine.get(), waveFileName));
		musicData.musicVolume[trackNo] = volume;
	}

	//--------------------------------
	//  音楽解放
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	void MonoAudio::UnloadMusic(int trackNo)
	{
		if (musicData.musicInst[trackNo]) musicData.musicInst[trackNo].reset();
		musicData.music[trackNo].reset(nullptr);
	}

	//--------------------------------
	//  音楽再生
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	//  bool isLoop                     ループ再生するかどうか
	void MonoAudio::PlayMusic(int trackNo, bool isLoop)
	{
		//if (!musicData.music[trackNo]) return;
		//musicData.musicInst[trackNo] = musicData.music[trackNo]->CreateInstance();
		//musicData.musicInst[trackNo]->Play(isLoop);
		//musicData.musicInst[trackNo]->SetVolume(musicData.musicVolume[trackNo]);

		if (!musicData.music[trackNo]) return;
		musicData.musicInst[trackNo] = musicData.music[trackNo]->CreateInstance(DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters);
		musicData.musicInst[trackNo]->Play(isLoop);
		musicData.musicInst[trackNo]->SetVolume(musicData.musicVolume[trackNo]);

		//listener.SetPosition(DirectX::XMFLOAT3(0, 0, 0));
		//emitter.SetPosition(DirectX::XMFLOAT3(0, 0, 1));

		//musicData.musicInst[trackNo]->Apply3D(listener, emitter);
	}

	//--------------------------------
	//  音楽停止
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	void MonoAudio::StopMusic(int trackNo)
	{
		if (!musicData.musicInst[trackNo]) return;
		musicData.musicInst[trackNo]->Stop();
	}

	//--------------------------------
	//  音楽一時停止
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	void MonoAudio::PauseMusic(int trackNo)
	{
		if (!musicData.musicInst[trackNo]) return;
		musicData.musicInst[trackNo]->Pause();
	}

	//--------------------------------
	//  音楽再開
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	void MonoAudio::ResumeMusic(int trackNo)
	{
		if (!musicData.musicInst[trackNo]) return;
		musicData.musicInst[trackNo]->Resume();
	}

	//--------------------------------
	//  音楽ボリューム設定
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	//  float volume                    ボリューム (0.0f ~ 1.0f)
	void MonoAudio::SetMusicVolume(int trackNo, float volume)
	{
		if (!musicData.musicInst[trackNo]) return;
		musicData.musicInst[trackNo]->SetVolume(volume);
	}

	//--------------------------------
	//  SoundStateの取得
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	SoundState MonoAudio::GetMusicState(int trackNo)
	{
		return static_cast<SoundState>(musicData.musicInst[trackNo]->GetState());
	}

	//--------------------------------
	//  ループの有無の取得
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	bool MonoAudio::IsMusicLooped(int trackNo)
	{
		return musicData.musicInst[trackNo]->IsLooped();
	}

	//--------------------------------
	//  panの設定
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	//  float pan                       pan (-1.0f ~ 1.0f)
	void MonoAudio::SetMusicPan(int trackNo, float pan)
	{
		return musicData.musicInst[trackNo]->SetPan(pan);
	}

	//--------------------------------
	//  pitchの設定
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	//  float pitch                     pitch (-1.0f ~ 1.0f)
	void MonoAudio::SetMusicPitch(int trackNo, float pitch)
	{
		return musicData.musicInst[trackNo]->SetPitch(pitch);
	}

	//--------------------------------
	//  フォーマットの取得
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	const WAVEFORMATEX* MonoAudio::GetMusicFormat(int trackNo)
	{
		return musicData.music[trackNo]->GetFormat();
	}

	//--------------------------------
	//  音楽が使用中かどうか
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 31)
	bool MonoAudio::IsInUseMusic(int trackNo)
	{
		return musicData.music[trackNo]->IsInUse();
	}




	//--------------------------------
	//  効果音の読み込み（xwbファイル）
	//--------------------------------
	//  const wchar_t* xwbFileName      xwbファイルのパス
	//  float volume                    ボリューム (0.0f ~ 1.0f)
	void MonoAudio::LoadSound(const wchar_t* xwbFileName, float volume)
	{
		for (int i = 0; i < WAVE_FILE_MAX; ++i)
		{
			for (int j = 0; j < WAVE_SOUND_MAX; ++j)
			{
				if (soundData.soundInst[i][j])
				{
					soundData.soundInst[i][j].reset();
				}
			}
		}

		soundData.waveBank.reset(new WaveBank(audioEngine.get(), xwbFileName));

		for (auto& p : soundData.soundVolume) p = volume;
	}

	//--------------------------------
	//  効果音の再生
	//--------------------------------
	// int	:	trackNo	トラック番号 (0 ~ 128)
	// float	:	pitch
	// float	:	pan
	// bool	:	loop	ループフラグ
	void MonoAudio::PlaySoundE(int trackNo, float pitch, float pan, bool looping)
	{
		for (int i = 0; i < WAVE_SOUND_MAX; ++i)
		{
			if (soundData.soundInst[trackNo][i])
			{
				DirectX::SoundState state = soundData.soundInst[trackNo][i]->GetState();
				if (state != DirectX::SoundState::STOPPED)continue;
			}

			soundData.soundInst[trackNo][i] = soundData.waveBank->CreateInstance(trackNo);
			if (soundData.soundInst[trackNo][i])
			{
				soundData.soundInst[trackNo][i]->SetVolume(soundData.soundVolume[trackNo]);
				soundData.soundInst[trackNo][i]->Play(looping);
				soundData.soundInst[trackNo][i]->SetPitch(pitch);
				soundData.soundInst[trackNo][i]->SetPan(pan);
				break;
			}

		}
	}

	//------------------------------------------------------
	//  効果音の停止
	//------------------------------------------------------
	//  int	:	trackNo	トラック番号 (0 ~ 128)
	void MonoAudio::StopSound(int trackNo)
	{
		for (int i = 0; i < WAVE_SOUND_MAX; ++i)
		{
			if (soundData.soundInst[trackNo][i])
			{
				DirectX::SoundState state = soundData.soundInst[trackNo][i]->GetState();
				if (state == DirectX::SoundState::STOPPED)continue;

				soundData.soundInst[trackNo][i]->Stop();
			}
		}
	}

	//------------------------------------------------------
	//  効果音の一時停止
	//------------------------------------------------------
	//  int	:	trackNo	トラック番号 (0 ~ 128)
	void MonoAudio::PauseSound(int trackNo)
	{
		for (int i = 0; i < WAVE_SOUND_MAX; ++i)
		{
			if (soundData.soundInst[trackNo][i])
			{
				DirectX::SoundState state = soundData.soundInst[trackNo][i]->GetState();
				if (state != DirectX::SoundState::PLAYING)continue;

				soundData.soundInst[trackNo][i]->Pause();
			}
		}
	}

	//------------------------------------------------------
	//  効果音の一時再開(一時停止から)
	//------------------------------------------------------
	//  int	:	trackNo	トラック番号 (0 ~ 128)
	void MonoAudio::ResumeSound(int trackNo)
	{
		for (int i = 0; i < WAVE_SOUND_MAX; ++i)
		{
			if (soundData.soundInst[trackNo][i])
			{
				DirectX::SoundState state = soundData.soundInst[trackNo][i]->GetState();
				if (state != DirectX::SoundState::PAUSED)continue;

				soundData.soundInst[trackNo][i]->Resume();
			}
		}
	}

	//--------------------------------
	//  効果音のボリューム設定
	//--------------------------------
	//  int trackNo                     トラック番号 (0 ~ 128)
	//  float volume                    ボリューム (0.0f ~ 1.0f)
	void MonoAudio::SetSoundVolume(int trackNo, float vol)
	{
		soundData.soundVolume[trackNo] = vol;
	}


	//------------------------------------------------------
	//  指定したトラックのいずれかの効果音のループ状態を取得
	//------------------------------------------------------
	// IN	int	:	トラック番号
	// OUT	bool	:	同一トラックの1つでも有効ならtrueを返す
	bool MonoAudio::IsSoundLooped(int trackNo)
	{
		assert(trackNo < WAVE_SOUND_MAX);

		for (int i = 0; i < WAVE_SOUND_MAX; ++i)
		{
			if (soundData.soundInst[trackNo][i])
			{
				if (soundData.soundInst[trackNo][i]->IsLooped() == true)
					return true;
			}
		}
		return false;
	}

}
