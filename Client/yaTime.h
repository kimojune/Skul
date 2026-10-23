#pragma once
#include "CommonInclude.h"

namespace ya
{
	class Time
	{
	public:
		static void Initiailize();
		static void Update();
		static void Render(HDC hdc);

		// 게임 로직용 프레임 시간 = 실제 경과 시간(최대 MaxDeltaTime) x 시간 배율
		__forceinline static double DeltaTime() { return mDeltaTime; }
		// 시간 배율을 적용하지 않은 프레임 시간 (UI, 히트스톱처럼 배율과 무관하게 흘러야 하는 것)
		__forceinline static double UnscaledDeltaTime() { return mUnscaledDeltaTime; }

		// 1이 기본. 0이면 게임 로직이 멈추고, 0.5면 절반 속도로 흐른다
		static void SetTimeScale(double scale) { mTimeScale = (scale < 0.0) ? 0.0 : scale; }
		static double GetTimeScale() { return mTimeScale; }

		// 0보다 크면 실제 경과 시간 대신 이 값을 프레임 시간으로 쓴다 (자동 검증 재현용)
		// 이때는 프레임 제한(60 FPS 대기)을 하지 않는다. MaxDeltaTime 제한은 똑같이 적용된다.
		static void SetFixedDeltaTime(double deltaTime) { mFixedDeltaTime = deltaTime; }

		// 한 프레임이 길어져도(로딩, 창 드래그 등) 이보다 크게 움직이지 않는다 (72px 발판 관통 방지)
		static constexpr double MaxDeltaTime = 1.0 / 30.0;
		// 실제 플레이의 목표 프레임 간격 (자동 검증의 고정 프레임 시간과 같다)
		static constexpr double TargetFrameTime = 1.0 / 60.0;

	private:
		static void waitForNextFrame();

		static double mDeltaTime;
		static double mUnscaledDeltaTime;
		static double mRealDeltaTime;
		static double mTimeScale;
		static double mFixedDeltaTime;
		static double mSecond;
		static UINT mFrameCount;
		static bool mbTimerPeriodSet;
		static LARGE_INTEGER mCpuFrequency;
		static LARGE_INTEGER mPrevFrequency;
		static LARGE_INTEGER mCurFrequency;
	};
}
