#pragma once
#include "YamYamEngine.h"

namespace ya
{
	// 디버그 빌드 전용 화면 왼쪽 위 정보창: FPS, 씬 이름, 프레임 번호, 플레이어 위치.
	// Application::Render에서 모든 그리기가 끝난 뒤 #ifdef _DEBUG 안에서 부른다.
	// 프레임 번호는 시나리오의 프레임 번호와 같다 (스크린샷과 state.jsonl을 맞춰 볼 때 쓴다).
	class UIDebugOverlay
	{
	public:
		static void Render(HDC hdc);
	};
}
