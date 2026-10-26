#include "yaTime.h"
#include "yaApplication.h"

extern ya::Application application;

namespace ya
{
	double Time::mAccumulator = 0.0;
	double Time::mDeltaTime = 0.0l;
	double Time::mUnscaledDeltaTime = 0.0;
	double Time::mRealDeltaTime = 0.0;
	double Time::mTimeScale = 1.0;
	double Time::mFixedDeltaTime = 0.0;
	double Time::mSecond = 0.0f;
	UINT Time::mFrameCount = 0;
	bool Time::mbTimerPeriodSet = false;
	LARGE_INTEGER Time::mCpuFrequency = {};
	LARGE_INTEGER Time::mPrevFrequency = {};
	LARGE_INTEGER Time::mCurFrequency = {};

	void Time::Initiailize()
	{
		// CPU 고유 진동수 가져오기
		QueryPerformanceFrequency(&mCpuFrequency);

		// 프로그램이 처음 시작할때 진동수
		QueryPerformanceCounter(&mPrevFrequency);
	}

	UINT Time::Update()
	{
		// 실제 플레이만 60 FPS로 제한한다. 고정 프레임 시간(자동 검증)일 때는 기다리지 않는다.
		if (mFixedDeltaTime <= 0.0)
			waitForNextFrame();

		QueryPerformanceCounter(&mCurFrequency);

		double differnceFrequency = (double)(mCurFrequency.QuadPart - mPrevFrequency.QuadPart);

		mRealDeltaTime = differnceFrequency / mCpuFrequency.QuadPart;
		mPrevFrequency.QuadPart = mCurFrequency.QuadPart;

		if (mFixedDeltaTime > 0.0)
		{
			// 자동 검증: 실제 시간과 무관하게 화면 한 번에 로직 한 단계
			double deltaTime = (mFixedDeltaTime > MaxDeltaTime) ? MaxDeltaTime : mFixedDeltaTime;
			mUnscaledDeltaTime = deltaTime;
			mDeltaTime = deltaTime * mTimeScale;
			return 1;
		}

		// 실제 플레이: 흐른 시간을 쌓아 두고 StepTime씩 꺼내 쓴다.
		// 화면이 느리게 그려져도 로직을 여러 번 돌려서 게임 속도는 그대로 유지한다.
		// (로딩·창 드래그처럼 아주 긴 프레임은 MaxStepsPerFrame 만큼만 따라잡고 나머지는 버린다)
		mAccumulator += mRealDeltaTime;

		// 60 FPS 대기 오차로 0번·2번이 번갈아 나오지 않도록 0.5ms 여유를 둔다
		const double tolerance = 0.0005;
		UINT steps = 0;
		while (mAccumulator + tolerance >= StepTime && steps < MaxStepsPerFrame)
		{
			mAccumulator -= StepTime;
			steps++;
		}
		if (steps == MaxStepsPerFrame || mAccumulator < 0.0)
			mAccumulator = 0.0;

		mUnscaledDeltaTime = StepTime;
		mDeltaTime = StepTime * mTimeScale;
		return steps;
	}

	void Time::waitForNextFrame()
	{
		if (mCpuFrequency.QuadPart <= 0)
			return;

		// Sleep(1)이 1ms 근처로 깨어나도록 타이머 해상도를 올린다 (프로세스 종료 시 자동 복구)
		if (!mbTimerPeriodSet)
		{
			timeBeginPeriod(1);
			mbTimerPeriodSet = true;
		}

		const LONGLONG frameCount = (LONGLONG)(TargetFrameTime * (double)mCpuFrequency.QuadPart);
		const LONGLONG targetCount = mPrevFrequency.QuadPart + frameCount;

		while (true)
		{
			LARGE_INTEGER now = {};
			QueryPerformanceCounter(&now);

			LONGLONG remain = targetCount - now.QuadPart;
			if (remain <= 0)
				break;

			// 2ms 넘게 남았으면 잠들고, 마지막 조금은 돌면서 기다린다 (Sleep 오차 보정)
			double remainMs = (double)remain * 1000.0 / (double)mCpuFrequency.QuadPart;
			if (remainMs > 2.0)
				Sleep((DWORD)(remainMs - 1.0));
			else
				YieldProcessor();
		}
	}

	void Time::Render(HDC hdc)
	{
		// FPS는 시간 배율과 상관없이 실제 경과 시간으로 잰다
		mSecond += mRealDeltaTime;
		mFrameCount++;

		if (mSecond > 1.0f)
		{
			HWND hWnd = application.GetHwnd();

			wchar_t szFloat[50] = {};
			float FPS = (float)(mFrameCount / mSecond);
			swprintf_s(szFloat, 50, L"FPS : %d", (UINT)(FPS + 0.5f));
			//int iLen = wcsnlen_s(szFloat, 50);
			SetWindowText(hWnd, szFloat);

			mSecond = 0.0f;
			mFrameCount = 0;
		}
	}
}
