#pragma once

#include "../Math/math.h"

#define NOMINMAX
#include <windows.h>
#include <algorithm>

namespace Adollib
{
	class Time
	{
	public:
		Time() = default;
		~Time() = default;
		Time(const Time&) = delete;
		void operator=(const Time&) = delete;

		void initialize()
		{

		}

		float timeStamp() const
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

			//フレーム制限
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


		// time_scaleの値をセットする(0以上)
		void timeScale(float t)
		{
			time_scale = static_cast<double>((std::max)(0.0f, t));
		}

		// time_scaleの値を返す
		float timeScale() const
		{
			return static_cast<float>(time_scale);
		}

		// このフレームの開始時間
		float thisTime() const
		{
			return static_cast<float>(this_time);
		}

		// delta_timeを取得する
		float deltaTime() const
		{
			if (stopped)return 0;
			if (!is_float_fps) return static_cast<float>(frames_per_sec * seconds_per_count);
			return static_cast<float>(delta_time);
		}

		// unscaled_delta_timeを取得する
		float unscaledDeltaTime() const
		{
			return static_cast<float>(unscaled_delta_time);
		}

		// FPSを固定するかどうかを設定する
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
		double seconds_per_count = 0.0;		// タイマーカウントの周期[s]
		//固定フレーム用
		LONGLONG frames_per_sec;
		// カウント変数
		LONGLONG awake_count = 0;			// 起動時のカウント
		LONGLONG this_count = 0;			// 現在のカウント
		LONGLONG last_count = 0;			// 一つ前のカウント
		LONGLONG paused_count = 0;			// タイマーを止めていたカウント
		LONGLONG stop_count = 0;			// タイマーを止めた時のカウント

		double time_scale = 1.0;			// タイムスケール

		double this_time = 0.0;				// ゲーム開始時からこのフレームまでの時間
		double unscaled_thie_time = 0.0;	// time_scaleの影響を受けないthis_time
		double delta_time = -1.0;			// フレーム間隔時間
		double unscaled_delta_time = -1.0;	// time_scaleの影響を受けないdelta_time



		bool is_float_fps = true;			// 可変FPSならtrue
		bool stopped = false;				// タイマーを止めた時trueになる
	};
}