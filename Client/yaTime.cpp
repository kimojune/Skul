#include "yaTime.h"
#include "yaApplication.h"

extern ya::Application application;

namespace ya
{
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

	void Time::Update()
	{
		// 실제 플레이만 60 FPS로 제한한다. 고정 프레임 시간(자동 검증)일 때는 기다리지 않는다.
		if (mFixedDeltaTime <= 0.0)
			waitForNextFrame();

		QueryPerformanceCounter(&mCurFrequency);

		double differnceFrequency = (double)(mCurFrequency.QuadPart - mPrevFrequency.QuadPart);

		mRealDeltaTime = differnceFrequency / mCpuFrequency.QuadPart;

		double deltaTime = mRealDeltaTime;
		if (mFixedDeltaTime > 0.0)
			deltaTime = mFixedDeltaTime;

		// 긴 프레임(로딩, 창 드래그, 대화상자)에서 한 번에 크게 움직여 발판을 뚫지 않도록 제한
		if (deltaTime > MaxDeltaTime)
			deltaTime = MaxDeltaTime;

		mUnscaledDeltaTime = deltaTime;
		mDeltaTime = deltaTime * mTimeScale;

		mPrevFrequency.QuadPart = mCurFrequency.QuadPart;
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
