#pragma once
#include "YamYamEngine.h"
#include "yaRenderer.h"


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

		// 그리기 백엔드. Initialize 전에 고른다 (--renderer gdi|dx11, 기본 dx11)
		void SetRendererType(eRendererType type) { mRendererType = type; }
		eRendererType GetRendererType() { return mRendererType; }

		// 자동 검증 모드. 둘 다 창을 숨기고 소리를 끈 채 고정 프레임 시간(1/60초)으로 돈다.
		void SetSmokeTest(const std::wstring& shotDir);
		bool LoadScenario(const std::wstring& path, const std::wstring& outDir);
		bool IsHeadless() { return mHeadless; }
		void SaveScreenshot(const std::wstring& path);

	private:
		void updateSmokeTest();

	private:
		//main
		HWND mHwnd;
		HDC mHdc;
		
		HMENU mMenubar;

		//tool
		HWND mToolHwnd;

		//그리기 (백버퍼는 Renderer의 백엔드가 가진다)
		eRendererType mRendererType;

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