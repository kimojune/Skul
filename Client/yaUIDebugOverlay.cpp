#include "yaUIDebugOverlay.h"

// 릴리스 빌드에서는 통째로 빠진다 (Application::Render의 호출도 #ifdef _DEBUG 안에 있다)
#ifdef _DEBUG
#include "yaUIDraw.h"
#include "yaTime.h"
#include "yaSceneManager.h"
#include "yaSkul.h"
#include "yaPlayeScene.h"
#include "yaTransform.h"
#include "yaApplication.h"
#include "yaRenderer.h"

extern ya::Application application;

namespace ya
{
	// eSceneType 순서
	static const wchar_t* UIDebugOverlaySceneNames[(UINT)eSceneType::End] =
	{
		L"Title", L"Stage1", L"Stage2", L"Stage3", L"Boss", L"Ending", L"Tool",
	};

	void UIDebugOverlay::Render(HDC hdc)
	{
		// FPS는 0.5초마다 평균을 낸다. 실제 플레이는 화면 한 번 그리는 데 걸린 실제 시간으로 재고,
		// 자동 검증(--smoke, --scenario)은 스크린샷이 매번 같도록 고정 프레임 시간으로 잰다 (항상 60)
		static UINT frame = 0;
		static UINT fpsFrames = 0;
		static double fpsTime = 0.0;
		static double fps = 0.0;

		double frameTime = application.IsHeadless() ? Time::DeltaTime() : Time::RealDeltaTime();
		fpsFrames++;
		fpsTime += frameTime;
		if (fps == 0.0 && frameTime > 0.0)
			fps = 1.0 / frameTime;
		if (fpsTime >= 0.5)
		{
			fps = fpsFrames / fpsTime;
			fpsFrames = 0;
			fpsTime = 0.0;
		}

		const int lineCount = 5;
		wchar_t lines[lineCount][160] = {};
		swprintf_s(lines[0], L"FPS %.1f (%.2f ms)", fps, fps > 0.0 ? 1000.0 / fps : 0.0);

		eSceneType sceneType = SceneManager::GetActiveSceneType();
		const wchar_t* sceneName = (sceneType < eSceneType::End) ? UIDebugOverlaySceneNames[(UINT)sceneType] : L"-";
		swprintf_s(lines[1], L"Scene %ls  frame %u", sceneName, frame);

		PlayeScene* playScene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
		Skul* skul = (playScene != nullptr) ? playScene->GetSkul() : nullptr;
		if (skul != nullptr)
		{
			Vector2 pos = skul->GetComponent<Transform>()->GetPos();
			swprintf_s(lines[2], L"Player %.1f, %.1f", pos.x, pos.y);
		}
		else
		{
			swprintf_s(lines[2], L"Player -");
		}

		// 그리기 백엔드와 GPU, 지난 프레임 전체(이 정보창 포함)에서 그린 양
		swprintf_s(lines[3], L"Renderer %ls  %ls", Renderer::GetName(), Renderer::GetAdapterName().c_str());
		const RenderStats& stats = Renderer::GetLastFrameStats();
		swprintf_s(lines[4], L"Draw calls %u  (sprites %u, 2D %u, culled %u)", stats.drawCalls, stats.sprites, stats.shapes, stats.culled);

		UIDraw::TextStyle style;
		style.face = L"Consolas";
		style.size = 16;
		const int lineHeight = 18;

		int width = 200;
		for (const wchar_t* line : lines)
		{
			SIZE size = UIDraw::MeasureLabel(hdc, line, style);
			if (width < size.cx)
				width = size.cx;
		}
		UIDraw::DrawShade(hdc, 8, 8, width + 16, lineHeight * lineCount + 10, RGB(0, 0, 0), 150);

		for (int i = 0; i < lineCount; i++)
		{
			style.color = RGB(235, 235, 235);
			if (i == 0)
			{
				// 초록 55 이상, 노랑 30 이상, 빨강 그 아래
				if (fps >= 55.0)
					style.color = RGB(120, 230, 120);
				else if (fps >= 30.0)
					style.color = RGB(240, 210, 90);
				else
					style.color = RGB(240, 90, 90);
			}
			UIDraw::DrawLabel(hdc, 16, 13 + i * lineHeight, lines[i], style);
		}

		frame++;
	}
}
#endif
