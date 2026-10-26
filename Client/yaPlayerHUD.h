#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Image;

	// 좌하단 플레이어 HUD (원작 배치: docs/ai-rework/04-original-reference.md "HUD")
	//  - 현재 스컬 큰 초상화, 교대 슬롯(SPACE, 두 번째 스컬이 없어 회색)
	//  - A·S 스킬 칸 (키 글자, 남은 쿨다운 초)
	//  - HP 바 + "현재 / 최대" 숫자, 방금 깎인 체력 잔상
	//  - 맞으면 화면 가장자리가 잠깐 붉어진다
	// 플레이 씬마다 UI 레이어에 하나씩 둔다 (PlayeScene::Initialize). 그리기는 UIDraw만 쓴다.
	class PlayerHUD : public GameObject
	{
	public:
		PlayerHUD();
		~PlayerHUD();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

	private:
		void renderPortrait(HDC hdc);
		void renderSkill(HDC hdc, int x, int y, const wchar_t* key, bool skillA);
		void renderHPBar(HDC hdc);
		void renderDamageFlash(HDC hdc);

		Image* mHeadImage;		// SkulHead.bmp (초상화, A 아이콘)
		Image* mHeadlessSheet;	// RightHeadless.bmp (머리 없을 때 초상화, S 아이콘)

		int mLastHP;
		float mTrailHP;			// 잔상이 가리키는 HP (깎인 뒤 잠깐 머물렀다가 따라 내려간다)
		float mTrailDelay;
		float mDamageFlash;		// 남은 붉은 테두리 시간
	};
}
