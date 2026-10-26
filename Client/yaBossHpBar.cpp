#include "yaBossHpBar.h"
#include "yaChapter1_Boss.h"
#include "yaUIDraw.h"
#include "yaTime.h"
#include "yaApplication.h"

extern ya::Application application;

namespace ya
{
	static const int BossHpBarWidth = 660;
	static const int BossHpBarHeight = 18;
	static const int BossHpBarTop = 54;

	// HP를 바 안쪽 픽셀 길이로 바꾼다. 왼쪽 반은 2페이즈 HP, 오른쪽 반은 1페이즈 HP
	static int BossHpBarPixels(float hp, int innerWidth)
	{
		int half = innerWidth / 2;
		if (hp <= 0.0f)
			return 0;
		if (hp <= (float)Chapter1_Boss::Phase2HP)
			return (int)(half * hp / Chapter1_Boss::Phase2HP + 0.5f);

		float phase1 = hp - (float)Chapter1_Boss::Phase2HP;
		if (phase1 > (float)Chapter1_Boss::Phase1HP)
			phase1 = (float)Chapter1_Boss::Phase1HP;
		return half + (int)((innerWidth - half) * phase1 / Chapter1_Boss::Phase1HP + 0.5f);
	}

	BossHpBar::BossHpBar()
		: mBoss(nullptr)
		, mShownHP((float)Chapter1_Boss::MaxHP)
		, mTrailDelay(0.0f)
		, mLastHP(Chapter1_Boss::MaxHP)
	{
	}
	BossHpBar::~BossHpBar()
	{
	}
	void BossHpBar::Initialize()
	{
		GameObject::Initialize();
	}
	void BossHpBar::Update()
	{
		if (mBoss == nullptr)
			return;

		// 맞으면 0.35초 뒤부터 흰 잔상이 초당 최대 HP의 60%씩 따라 내려온다
		int hp = mBoss->GetHP();
		if (hp < mLastHP)
			mTrailDelay = 0.35f;
		mLastHP = hp;

		float dt = (float)Time::DeltaTime();
		if (mTrailDelay > 0.0f)
		{
			mTrailDelay -= dt;
		}
		else if (mShownHP > hp)
		{
			mShownHP -= Chapter1_Boss::MaxHP * 0.6f * dt;
			if (mShownHP < hp)
				mShownHP = (float)hp;
		}
		if (mShownHP < hp)
			mShownHP = (float)hp;

		GameObject::Update();
	}
	void BossHpBar::Render(HDC hdc)
	{
		if (mBoss == nullptr || !mBoss->IsBarVisible())
			return;

		int centerX = (int)application.GetWidth() / 2;
		int x = centerX - BossHpBarWidth / 2;
		int y = BossHpBarTop;

		// 글자가 배경에 묻히지 않게 옅은 검은 판
		UIDraw::DrawShade(hdc, x - 16, 14, BossHpBarWidth + 32, 92, RGB(0, 0, 0), 110);

		UIDraw::TextStyle nameStyle;
		nameStyle.size = 26;
		nameStyle.bold = true;
		nameStyle.align = UIDraw::eAlign::Center;
		nameStyle.shadow = true;
		nameStyle.color = RGB(250, 240, 225);
		UIDraw::DrawLabel(hdc, centerX, 20, L"Yggdrassil", nameStyle);

		// 테두리와 빈 칸
		UIDraw::DrawBox(hdc, x, y, BossHpBarWidth, BossHpBarHeight, RGB(32, 22, 28), RGB(8, 6, 6), 2);

		int innerX = x + 2;
		int innerY = y + 2;
		int innerWidth = BossHpBarWidth - 4;
		int innerHeight = BossHpBarHeight - 4;
		int half = innerWidth / 2;

		int hpPixels = BossHpBarPixels((float)mBoss->GetHP(), innerWidth);
		int trailPixels = BossHpBarPixels(mShownHP, innerWidth);

		if (trailPixels > hpPixels)
			UIDraw::DrawBox(hdc, innerX + hpPixels, innerY, trailPixels - hpPixels, innerHeight, RGB(240, 240, 240), RGB(240, 240, 240), 0);

		// 왼쪽 칸(2페이즈 HP): 1페이즈 동안은 어두운 빨강, 2페이즈가 되면 보라빛 빨강
		COLORREF leftColor = (mBoss->GetPhase() >= 2) ? RGB(196, 46, 140) : RGB(150, 30, 40);
		COLORREF rightColor = RGB(214, 46, 46);
		int leftPixels = hpPixels < half ? hpPixels : half;
		if (leftPixels > 0)
			UIDraw::DrawBox(hdc, innerX, innerY, leftPixels, innerHeight, leftColor, leftColor, 0);
		if (hpPixels > half)
			UIDraw::DrawBox(hdc, innerX + half, innerY, hpPixels - half, innerHeight, rightColor, rightColor, 0);

		// 페이즈 경계 표시
		UIDraw::DrawBox(hdc, innerX + half - 1, y - 4, 3, BossHpBarHeight + 8, RGB(235, 200, 120), RGB(235, 200, 120), 0);

		UIDraw::TextStyle titleStyle;
		titleStyle.size = 17;
		titleStyle.align = UIDraw::eAlign::Center;
		titleStyle.shadow = true;
		titleStyle.color = RGB(215, 200, 175);
		UIDraw::DrawLabel(hdc, centerX, y + BossHpBarHeight + 6, L"Elder Ent", titleStyle);

		GameObject::Render(hdc);
	}
	void BossHpBar::Release()
	{
		GameObject::Release();
	}
}
