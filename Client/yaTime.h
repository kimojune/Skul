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

		__forceinline static double DeltaTime() { return mDeltaTime; }

		// 0보다 크면 실제 경과 시간 대신 이 값을 프레임 시간으로 쓴다 (자동 검증 재현용)
		static void SetFixedDeltaTime(double deltaTime) { mFixedDeltaTime = deltaTime; }

	private:
		static double mDeltaTime;
		static double mFixedDeltaTime;
		static double mSecond;
		static LARGE_INTEGER mCpuFrequency;
		static LARGE_INTEGER mPrevFrequency;
		static LARGE_INTEGER mCurFrequency;
	};
}
