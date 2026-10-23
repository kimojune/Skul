#include "yaApplication.h"
#include "yaSceneManager.h"
#include "yaTime.h"
#include "yaInput.h"
#include "yaCollisionManager.h"
#include "yaCamera.h"
#include "Resource.h"
#include "yaSoundManager.h"

namespace ya
{
	Application::Application()
		: mHwnd(NULL)
		, mHdc(NULL)
		, mSmokeTest(false)
		, mSmokeStep(0)
		, mSmokeTime(0.0)
	{
	}

	Application::~Application()
	{
		//SceneManager::Release();
		//Time::Release();
	}

	void Application::Initialize(HWND hWnd)
	{
		mHwnd = hWnd;
		mHdc = GetDC(hWnd);
		mWidth = 1600;
		mHeight = 900;

		//비트맵 해상도를 설정하기 위한 실제 윈도우 크기를 계산해준다.
		RECT rect = { 0, 0, mWidth , mHeight };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

		// 윈도우 크기 변경및 출력 설정 (smoke 검증 중에는 창을 숨긴 채로 둔다)
		if (!mSmokeTest)
		{
			SetWindowPos(mHwnd
				, nullptr, 0, 0
				, rect.right - rect.left
				, rect.bottom - rect.top
				, 0);
			ShowWindow(hWnd, true);
		}

		mBackBuffer = CreateCompatibleBitmap(mHdc, mWidth, mHeight);
		mBackHDC = CreateCompatibleDC(mHdc);
		mMenubar = LoadMenu(nullptr, MAKEINTRESOURCE(IDI_CLIENT));

		HBITMAP defaultBitmap	
			= (HBITMAP)SelectObject(mBackHDC, mBackBuffer);
		DeleteObject(defaultBitmap);

		Time::Initiailize();
		Input::Initialize();
		SoundManager::SetMute(mSmokeTest);
		SoundManager::Initialize();
		SceneManager::Initialize();
		Camera::Initialize();

		SetMenuBar(false);
	}

	void Application::Run()
	{
		Update();
		Render();
		SceneManager::Destroy();

		if (mSmokeTest)
			updateSmokeTest();
	}

	void Application::Update()
	{
		Time::Update();
		Input::Update();
		SceneManager::Update();
		CollisionManager::Update();
		Camera::Update();
	}

	void Application::Render()
	{
		// clear
		clear();

		Time::Render(mBackHDC);
		Input::Render(mBackHDC);
		SceneManager::Render(mBackHDC);
		Camera::Render(mBackHDC);
		// 백버퍼에 있는 그림을 원본버퍼에 그려줘야한다.
		BitBlt(mHdc, 0, 0, mWidth, mHeight, mBackHDC, 0, 0, SRCCOPY);
	}
	void Application::clear()
	{
		HBRUSH grayBrush = CreateSolidBrush(RGB(121, 121, 121));
		HBRUSH oldBrush = (HBRUSH)SelectObject(mBackHDC, grayBrush);
		Rectangle(mBackHDC, -1, -1, 1602, 902);
		SelectObject(mBackHDC, oldBrush);
		DeleteObject(grayBrush);
	}
	void Application::updateSmokeTest()
	{
		// 씬마다 머무는 시간. 보스는 5초 주기 패턴이 한 번 이상 돌도록 길게 둔다.
		struct SmokeStep
		{
			eSceneType scene;
			const wchar_t* name;
			double seconds;
		};
		static const SmokeStep steps[] =
		{
			{ eSceneType::Title, L"Title", 1.0 },
			{ eSceneType::Stage1, L"Stage1", 2.0 },
			{ eSceneType::Stage2, L"Stage2", 3.0 },
			{ eSceneType::Boss, L"Boss", 12.0 },
		};

		if (mSmokeStep >= _countof(steps))
			return;

		mSmokeTime += Time::DeltaTime();
		if (mSmokeTime < steps[mSmokeStep].seconds)
			return;

		// 씬을 넘기기 직전 화면(직전 프레임의 백버퍼)을 남긴다.
		if (!mSmokeShotDir.empty())
		{
			std::wstring path = mSmokeShotDir + L"\\" + std::to_wstring(mSmokeStep) + L"_" + steps[mSmokeStep].name + L".bmp";
			saveScreenshot(path);
		}

		mSmokeTime = 0.0;
		if (++mSmokeStep < _countof(steps))
			SceneManager::LoadScene(steps[mSmokeStep].scene);
		else
			PostQuitMessage(0); // 크래시 없이 끝까지 돌았으면 종료 코드 0
	}
	void Application::saveScreenshot(const std::wstring& path)
	{
		// 백버퍼는 DC에 선택된 상태라 직접 읽지 않고 DIB에 복사해서 24비트 BMP로 쓴다.
		BITMAPINFO info = {};
		info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		info.bmiHeader.biWidth = mWidth;
		info.bmiHeader.biHeight = mHeight;
		info.bmiHeader.biPlanes = 1;
		info.bmiHeader.biBitCount = 24;
		info.bmiHeader.biCompression = BI_RGB;

		void* bits = nullptr;
		HBITMAP dib = CreateDIBSection(mBackHDC, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
		if (dib == nullptr)
			return;

		HDC dibDC = CreateCompatibleDC(mBackHDC);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(dibDC, dib);
		BitBlt(dibDC, 0, 0, mWidth, mHeight, mBackHDC, 0, 0, SRCCOPY);
		GdiFlush();

		DWORD imageSize = ((mWidth * 3 + 3) / 4) * 4 * mHeight;
		BITMAPFILEHEADER fileHeader = {};
		fileHeader.bfType = 0x4D42; // "BM"
		fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fileHeader.bfSize = fileHeader.bfOffBits + imageSize;

		FILE* file = nullptr;
		_wfopen_s(&file, path.c_str(), L"wb");
		if (file != nullptr)
		{
			fwrite(&fileHeader, sizeof(fileHeader), 1, file);
			fwrite(&info.bmiHeader, sizeof(BITMAPINFOHEADER), 1, file);
			fwrite(bits, imageSize, 1, file);
			fclose(file);
		}

		SelectObject(dibDC, oldBitmap);
		DeleteDC(dibDC);
		DeleteObject(dib);
	}
	void Application::SetMenuBar(bool power)
	{
		if (mSmokeTest)
			return;

		SetMenu(mHwnd, mMenubar);

		RECT rect = { 0, 0, mWidth , mHeight };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, power);

		// 윈도우 크기 변경및 출력 설정
		SetWindowPos(mHwnd
			, nullptr, 0, 0
			, rect.right - rect.left
			, rect.bottom - rect.top
			, 0);
		ShowWindow(mHwnd, true);
	}
}
