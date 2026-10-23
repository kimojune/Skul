#pragma once
#include "YamYamEngine.h"


namespace ya
{
	class Scenario;

	class Application
	{
	public:
		Application();
		~Application();

		void Initialize(HWND hWnd);
		void Run();
		void Update();
		void Render();

		HWND GetHwnd() { return mHwnd; }
		HWND GetToolHwnd() { return mToolHwnd; }
		HDC GetHdc() { return mHdc; }
		UINT GetWidth() { return mWidth; }
		UINT GetHeight() { return mHeight; }

		void SetToolHwnd(HWND hwnd) { mToolHwnd = hwnd;}
		void SetMenuBar(bool power);

		// 자동 검증 모드. 둘 다 창을 숨기고 소리를 끈 채 고정 프레임 시간(1/60초)으로 돈다.
		void SetSmokeTest(const std::wstring& shotDir);
		bool LoadScenario(const std::wstring& path, const std::wstring& outDir);
		bool IsHeadless() { return mHeadless; }
		void SaveScreenshot(const std::wstring& path);

	private:
		void clear();
		void updateSmokeTest();

	private:
		//main
		HWND mHwnd;
		HDC mHdc;
		
		HMENU mMenubar;

		//tool
		HWND mToolHwnd;

		//백버퍼
		HBITMAP mBackBuffer;
		HDC mBackHDC;

		UINT mWidth;
		UINT mHeight;

		Vector2 mPos;

		//자동 검증 (--smoke, --scenario)
		bool mHeadless;
		Scenario* mScenario;
		bool mSmokeTest;
		UINT mSmokeStep;
		double mSmokeTime;
		std::wstring mSmokeShotDir;
	};
}