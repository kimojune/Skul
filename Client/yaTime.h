#pragma once
#include "CommonInclude.h"

namespace ya
{
	class Time
	{
	public:
		static void Initiailize();
		// 이번 화면 프레임에서 게임 로직을 몇 번(StepTime 단위) 진행할지 돌려준다.
		// 실제 플레이는 흐른 시간을 쌓아 두고 1/60초씩 꺼내 쓰므로, 화면이 30 FPS로 그려지면 로직은 2번 돈다.
		// 고정 프레임 시간(자동 검증)일 때는 항상 1이다.
		static UINT Update();
		static void Render(HDC hdc);

		// 게임 로직 한 단계의 시간 = StepTime(자동 검증은 고정값, 최대 MaxDeltaTime) x 시간 배율
		__forceinline static double DeltaTime() { return mUnscaledDeltaTime * mTimeScale; }
		// 시간 배율을 적용하지 않은 한 단계의 시간 (UI, 히트스톱처럼 배율과 무관하게 흘러야 하는 것)
		__forceinline static double UnscaledDeltaTime() { return mUnscaledDeltaTime; }
		// 직전 화면 프레임에 실제로 걸린 시간 (FPS 표시용)
		__forceinline static double RealDeltaTime() { return mRealDeltaTime; }

		// 1이 기본. 0이면 게임 로직이 멈추고, 0.5면 절반 속도로 흐른다
		static void SetTimeScale(double scale) { mTimeScale = (scale < 0.0) ? 0.0 : scale; }
		static double GetTimeScale() { return mTimeScale; }

		// 0보다 크면 실제 경과 시간 대신 이 값을 프레임 시간으로 쓴다 (자동 검증 재현용)
		// 이때는 프레임 제한(60 FPS 대기)을 하지 않는다. MaxDeltaTime 제한은 똑같이 적용된다.
		static void SetFixedDeltaTime(double deltaTime) { mFixedDeltaTime = deltaTime; }

		// 로직 한 단계가 이보다 크게 움직이지 않는다 (72px 발판 관통 방지)
		static constexpr double MaxDeltaTime = 1.0 / 30.0;
		// 실제 플레이의 목표 프레임 간격이자 로직 한 단계의 시간 (자동 검증의 고정 프레임 시간과 같다)
		static constexpr double TargetFrameTime = 1.0 / 60.0;
		static constexpr double StepTime = TargetFrameTime;
		// 화면이 아주 느려도(12 FPS 미만) 한 번에 이 이상은 따라잡지 않는다. 그보다 느리면 게임이 느려진다
		static constexpr UINT MaxStepsPerFrame = 5;

	private:
		static void waitForNextFrame();

		static double mAccumulator;
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
