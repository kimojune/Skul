#include "yaApplication.h"
#include "yaSceneManager.h"
#include "yaTime.h"
#include "yaInput.h"
#include "yaCollisionManager.h"
#include "yaCamera.h"
#include "Resource.h"
#include "yaSoundManager.h"
#include "yaScenario.h"
#include "yaUIDebugOverlay.h"

namespace ya
{
	Application::Application()
		: mHwnd(NULL)
		, mHdc(NULL)
		, mRendererType(eRendererType::GDI)
		, mHeadless(false)
		, mScenario(nullptr)
		, mSmokeTest(false)
		, mSmokeStep(0)
		, mSmokeTime(0.0)
	{
	}

	Application::~Application()
	{
		//SceneManager::Release();
		//Time::Release();
		delete mScenario;
		Renderer::Release();
	}

	void Application::SetSmokeTest(const std::wstring& shotDir)
	{
		mHeadless = true;
		mSmokeTest = true;
		mSmokeShotDir = shotDir;
		Time::SetFixedDeltaTime(1.0 / 60.0);
	}

	bool Application::LoadScenario(const std::wstring& path, const std::wstring& outDir)
	{
		mHeadless = true;
		Time::SetFixedDeltaTime(1.0 / 60.0); // 대본의 dt 명령이 있으면 Load에서 덮어쓴다
		mScenario = new Scenario();
		return mScenario->Load(path, outDir);
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

		// 윈도우 크기 변경및 출력 설정 (자동 검증 중에는 창을 숨긴 채로 둔다)
		if (!mHeadless)
		{
			SetWindowPos(mHwnd
				, nullptr, 0, 0
				, rect.right - rect.left
				, rect.bottom - rect.top
				, 0);
			ShowWindow(hWnd, true);
		}

		Renderer::Initialize(mRendererType, mHwnd, mWidth, mHeight);
		mMenubar = LoadMenu(nullptr, MAKEINTRESOURCE(IDI_CLIENT));

		Time::Initiailize();
		Input::Initialize();
		SoundManager::SetMute(mHeadless);
		SoundManager::Initialize();
		SceneManager::Initialize();
		Camera::Initialize();

		SetMenuBar(false);

		if (mScenario != nullptr)
			mScenario->Start();
	}

	void Application::Run()
	{
		// 게임 로직은 항상 1/60초 단위로 진행하고, 화면은 그 결과를 한 번 그린다.
		// 화면이 느리게 그려지는 PC에서도 게임 속도와 물리 결과가 자동 검증과 같다.
		UINT steps = Time::Update();
		for (UINT i = 0; i < steps; i++)
		{
			if (mScenario != nullptr)
				mScenario->BeginFrame();

			Update();
			SceneManager::Destroy();
		}

		Render();

		if (mSmokeTest)
			updateSmokeTest();
		if (mScenario != nullptr)
			mScenario->EndFrame();
	}

	void Application::Update()
	{
		Input::Update();
		SceneManager::Update();
		CollisionManager::Update();
		Camera::Update();
	}

	void Application::Render()
	{
		// clear
		Renderer::BeginFrame(RGB(121, 121, 121));

		// GDI는 백버퍼 DC, DX11은 쓰지 않는 DC. 그리기는 모두 Renderer를 거친다
		HDC hdc = Renderer::GetHdc();
		Time::Render(hdc);
		Input::Render(hdc);
		SceneManager::Render(hdc);
		Camera::Render(hdc);
#ifdef _DEBUG
		// 디버그 정보창은 페이드 효과보다도 위에 그린다
		UIDebugOverlay::Render(hdc);
#endif
		// 백버퍼에 있는 그림을 화면에 내보낸다.
		Renderer::EndFrame();
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
			SaveScreenshot(path);
		}

		mSmokeTime = 0.0;
		if (++mSmokeStep < _countof(steps))
			SceneManager::LoadScene(steps[mSmokeStep].scene);
		else
			PostQuitMessage(0); // 크래시 없이 끝까지 돌았으면 종료 코드 0
	}
	void Application::SaveScreenshot(const std::wstring& path)
	{
		// 마지막으로 그린 백버퍼를 24비트 BMP로 쓴다 (GDI는 백버퍼 비트맵, DX11은 GPU 렌더 타깃에서 읽는다)
		Renderer::SaveScreenshot(path);
	}
	void Application::SetMenuBar(bool power)
	{
		if (mHeadless)
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
