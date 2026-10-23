#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Chapter1_Boss;

	// 화면 위 가운데 보스 HP 바 (UI 레이어). 원작처럼 바 위에 이름, 아래에 칭호를 쓰고 숫자는 쓰지 않는다.
	// 이름은 원작 게임 화면의 철자 "Yggdrassil"을 그대로 쓴다 (위키 표기는 Yggdrasil).
	// 바는 두 칸이다. 오른쪽 칸이 1페이즈 HP라 먼저 줄고, 왼쪽 칸이 2페이즈 HP다.
	// 2페이즈가 되면 왼쪽 칸이 보라빛 붉은색으로 바뀐다.
	class BossHpBar : public GameObject
	{
	public:
		BossHpBar();
		~BossHpBar();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		void SetBoss(Chapter1_Boss* boss) { mBoss = boss; }

	private:
		Chapter1_Boss* mBoss;
		float mShownHP;		// 깎인 부분이 천천히 따라오는 값 (흰 잔상)
		float mTrailDelay;	// 맞은 뒤 잔상이 줄기 시작할 때까지 남은 시간
		int mLastHP;
	};
}
