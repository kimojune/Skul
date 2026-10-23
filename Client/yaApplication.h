#pragma once
#include "YamYamEngine.h"


namespace ya
{
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

		void SetSmokeTest(bool smokeTest, const std::wstring& shotDir) { mSmokeTest = smokeTest; mSmokeShotDir = shotDir; }
		bool IsSmokeTest() { return mSmokeTest; }

	private:
		void clear();
		void updateSmokeTest();
		void saveScreenshot(const std::wstring& path);

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

		//자동 검증 (--smoke)
		bool mSmokeTest;
		UINT mSmokeStep;
		double mSmokeTime;
		std::wstring mSmokeShotDir;
	};
}