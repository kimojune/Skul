#pragma once
#include "yaScene.h"

namespace ya
{
	class ToolScene : public Scene
	{
	public:
		ToolScene();
		~ToolScene();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		virtual void OnEnter() override;
		virtual void OnExit() override;

	private:
		// 왼쪽 버튼을 누른 채 끌 때, 마지막으로 칠한 칸 (같은 칸을 매 프레임 다시 칠하지 않는다)
		bool mbPainting;
		Vector2 mLastPaintCell;
	};
}