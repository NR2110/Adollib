#pragma once

#include "singleton.h"

#define NOMINMAX
#include <windows.h>
#include <algorithm>

namespace Adollib
{
	class Time : public Singleton<Time>
	{
	public:
		Time() = default;
		~Time() = default;
		Time(const Time&) = delete;
		void operator=(const Time&) = delete;

		void initialize()
		{
			LONGLONG counts_per_sec;
			QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));	// Hz���擾
			seconds_per_count = 1.0 / static_cast<double>(counts_per_sec);
			frames_per_sec = (LONGLONG)(counts_per_sec / 60.0);

			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_count));
			awake_count = this_count;
			last_count = this_count;
		}
		// Returns the total time elapsed since Reset() was called, NOT counting any
		// time when the clock is stopped.
		float timeStamp() const  // in seconds
		{
			// If we are stopped, do not count the time that has passed since we stopped.
			// Moreover, if we previously already had a pause, the distance 
			// stopTime - baseTime includes paused time, which we do not want to count.
			// To correct this, we can subtract the paused time from mStopTime:  
			//
			//                     |<--pausedTime-->|
			// ----*---------------*-----------------*------------*------------*------> time
			//  baseTime       stopTime        start_time     stopTime    thisTime

			if (stopped)
			{
				return static_cast<float>(((stop_count - paused_count) - awake_count) * seconds_per_count);
			}

			// The distance thisTime - mBaseTime includes paused time,
			// which we do not want to count.  To correct this, we can subtract 
			// the paused time from thisTime:  
			//
			//  (thisTime - pausedTime) - baseTime 
			//
			//                     |<--pausedTime-->|
			// ----*---------------*-----------------*------------*------> time
			//  baseTime       stopTime        start_time     thisTime
			else
			{
				return static_cast<float>(((this_count - paused_count) - awake_count) * seconds_per_count);
			}
		}

		void reset() // Call before message loop.
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_count));
			awake_count = this_count;
			last_count = this_count;

			stop_count = 0;
			stopped = false;
		}

		void start() // Call when unpaused.
		{
			LONGLONG start_time;
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&start_time));

			// Accumulate the time elapsed between stop and start pairs.
			//
			//                     |<-------d------->|
			// ----*---------------*-----------------*------------> time
			//  baseTime       stopTime        start_time     
			if (stopped)
			{
				paused_count += (start_time - stop_count);
				last_count = start_time;
				stop_count = 0;
				stopped = false;
			}
		}

		void stop() // Call when paused.
		{
			if (!stopped)
			{
				QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&stop_count));
				stopped = true;
			}
		}

		bool tick() // Call every frame.
		{
			if (stopped)
			{
				delta_time = 0.0;
				unscaled_delta_time = 0.0;
				return true;
			}

			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_count));

			//�t���[������
			if (!is_float_fps)
			{
				if (this_count < last_count + frames_per_sec) return false;
			}
			// Time difference between this frame and the previous.
			LONGLONG subtraction_delta = this_count - last_count;
			unscaled_delta_time = subtraction_delta * seconds_per_count;
			delta_time = subtraction_delta * time_scale * seconds_per_count;

			unscaled_thie_time += unscaled_delta_time;
			this_time += delta_time;
			// Prepare for next frame.
			last_count = this_count;

			// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
			// processor goes into a power save mode or we get shuffled to another
			// processor, then mdelta_time can be negative.
			if (delta_time < 0.0)
			{
				delta_time = 0.0;
			}
			return true;
		}


		// time_scale�̒l���Z�b�g����(0�ȏ�)
		void timeScale(float t)
		{
			time_scale = static_cast<double>((std::max)(0.0f, t));
		}
		// time_scale�̒l��Ԃ�
		float timeScale() const
		{
			return static_cast<float>(time_scale);
		}

		// ���̃t���[���̊J�n����
		float thisTime() const
		{
			return static_cast<float>(this_time);
		}

		// delta_time���擾����
		float deltaTime() const
		{
			return static_cast<float>(delta_time);
		}

		// unscaled_delta_time���擾����
		float unscaledDeltaTime() const
		{
			return static_cast<float>(unscaled_delta_time);
		}

		// FPS���Œ肷�邩�ǂ�����ݒ肷��
		void isFloatFPS(bool is_float)
		{
			is_float_fps = is_float;
		}

		void setFrameRate(double frameRate)
		{
			LONGLONG counts_per_sec;
			QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));
			seconds_per_count = 1.0 / static_cast<double>(counts_per_sec);
			frames_per_sec = (LONGLONG)(counts_per_sec / frameRate);
		}

	private:
		double seconds_per_count = 0.0;		// �^�C�}�[�J�E���g�̎���[s]
		//�Œ�t���[���p
		LONGLONG frames_per_sec;
		// �J�E���g�ϐ�
		LONGLONG awake_count = 0;			// �N�����̃J�E���g
		LONGLONG this_count = 0;			// ���݂̃J�E���g
		LONGLONG last_count = 0;			// ��O�̃J�E���g
		LONGLONG paused_count = 0;			// �^�C�}�[���~�߂Ă����J�E���g
		LONGLONG stop_count = 0;			// �^�C�}�[���~�߂����̃J�E���g

		double time_scale = 1.0;			// �^�C���X�P�[��

		double this_time = 0.0;				// �Q�[���J�n�����炱�̃t���[���܂ł̎���
		double unscaled_thie_time = 0.0;	// time_scale�̉e�����󂯂Ȃ�this_time
		double delta_time = -1.0;			// �t���[���Ԋu����
		double unscaled_delta_time = -1.0;	// time_scale�̉e�����󂯂Ȃ�delta_time



		bool is_float_fps = true;			// ��FPS�Ȃ�true
		bool stopped = false;				// �^�C�}�[���~�߂���true�ɂȂ�
	};
}