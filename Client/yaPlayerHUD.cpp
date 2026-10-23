#include "yaPlayerHUD.h"
#include "yaUIDraw.h"
#include "yaResources.h"
#include "yaImage.h"
#include "yaTime.h"
#include "yaPlayerStats.h"
#include "yaApplication.h"

extern ya::Application application;

namespace ya
{
	// 깎인 체력 잔상: 맞은 뒤 이만큼 머물렀다가 초당 PlayerHUDTrailSpeed HP씩 따라 내려간다
	static constexpr float PlayerHUDTrailHoldTime = 0.4f;
	static constexpr float PlayerHUDTrailSpeed = 60.0f;
	// 맞았을 때 화면 가장자리 붉은 테두리
	static constexpr float PlayerHUDDamageFlashTime = 0.3f;
	static constexpr int PlayerHUDDamageFlashWidth = 28;

	// 배치 (화면 1600x900, 좌하단)
	static constexpr int PlayerHUDPortraitX = 22;
	static constexpr int PlayerHUDPortraitY = 776;
	static constexpr int PlayerHUDPortraitSize = 100;
	static constexpr int PlayerHUDSkillX = 136;
	static constexpr int PlayerHUDSkillY = 780;
	static constexpr int PlayerHUDSkillSize = 52;
	static constexpr int PlayerHUDSkillGap = 10;
	static constexpr int PlayerHUDBarX = 136;
	static constexpr int PlayerHUDBarY = 846;
	static constexpr int PlayerHUDBarWidth = 296;
	static constexpr int PlayerHUDBarHeight = 24;

	static const COLORREF PlayerHUDFrameFill = RGB(38, 32, 46);
	static const COLORREF PlayerHUDFrameBorder = RGB(196, 170, 120);

	// RightHeadless.bmp 첫 칸(대기 1프레임)에서 몸이 그려진 영역
	static constexpr int PlayerHUDHeadlessSrcX = 51;
	static constexpr int PlayerHUDHeadlessSrcY = 53;
	static constexpr int PlayerHUDHeadlessSrcWidth = 36;
	static constexpr int PlayerHUDHeadlessSrcHeight = 23;

	PlayerHUD::PlayerHUD()
		: mHeadImage(nullptr)
		, mHeadlessSheet(nullptr)
		, mLastHP(0)
		, mTrailHP(0.0f)
		, mTrailDelay(0.0f)
		, mDamageFlash(0.0f)
	{
		SetName(L"PlayerHUD");
	}

	PlayerHUD::~PlayerHUD()
	{
	}

	void PlayerHUD::Initialize()
	{
		// 같은 이미지를 쓰는 SkulHead·Nohead와 같은 키로 불러 한 장만 둔다
		mHeadImage = Resources::Load<Image>(L"RightSkulHead", L"..\\Resources\\SkulHead.bmp");
		mHeadlessSheet = Resources::Load<Image>(L"RightHeadless", L"..\\Resources\\RightHeadless.bmp");

		mLastHP = PlayerStats::GetHP();
		mTrailHP = (float)mLastHP;

		GameObject::Initialize();
	}

	void PlayerHUD::Update()
	{
		float deltaTime = (float)Time::DeltaTime();

		int hp = PlayerStats::GetHP();
		if (hp < mLastHP)
		{
			// 잔상은 맞기 전 HP에 남겨 두고(연달아 맞으면 더 길어진다) 잠깐 기다린다
			mTrailDelay = PlayerHUDTrailHoldTime;
			mDamageFlash = PlayerHUDDamageFlashTime;
		}
		else if (hp > mLastHP)
		{
			// 회복·재시작은 잔상 없이 바로 채운다
			mTrailHP = (float)hp;
		}
		mLastHP = hp;

		if (mTrailHP > hp)
		{
			if (mTrailDelay > 0.0f)
			{
				mTrailDelay -= deltaTime;
			}
			else
			{
				mTrailHP -= PlayerHUDTrailSpeed * deltaTime;
				if (mTrailHP < hp)
					mTrailHP = (float)hp;
			}
		}

		mDamageFlash -= deltaTime;
		if (mDamageFlash < 0.0f)
			mDamageFlash = 0.0f;

		GameObject::Update();
	}

	void PlayerHUD::Render(HDC hdc)
	{
		renderDamageFlash(hdc);

		// 뒤판
		UIDraw::DrawShade(hdc, 10, 764, 432, 128, RGB(0, 0, 0), 130);

		renderPortrait(hdc);
		renderSkill(hdc, PlayerHUDSkillX, PlayerHUDSkillY, L"A", true);
		renderSkill(hdc, PlayerHUDSkillX + PlayerHUDSkillSize + PlayerHUDSkillGap, PlayerHUDSkillY, L"S", false);
		renderHPBar(hdc);

		GameObject::Render(hdc);
	}

	void PlayerHUD::Release()
	{
		GameObject::Release();
	}

	void PlayerHUD::renderPortrait(HDC hdc)
	{
		const int x = PlayerHUDPortraitX;
		const int y = PlayerHUDPortraitY;
		const int size = PlayerHUDPortraitSize;
		UIDraw::DrawBox(hdc, x, y, size, size, PlayerHUDFrameFill, PlayerHUDFrameBorder, 3);

		// 지금 스컬: 머리가 붙어 있으면 해골 얼굴, 던졌으면 머리 없는 몸
		if (!PlayerStats::IsHeadOut())
		{
			const float scale = 5.5f;
			int width = (int)(mHeadImage->GetWidth() * scale);
			int height = (int)(mHeadImage->GetHeight() * scale);
			UIDraw::DrawImage(hdc, mHeadImage, x + (size - width) / 2, y + (size - height) / 2, scale);
		}
		else
		{
			int width = (int)(PlayerHUDHeadlessSrcWidth * 2.4f);
			int height = (int)(PlayerHUDHeadlessSrcHeight * 2.4f);
			UIDraw::DrawImagePart(hdc, mHeadlessSheet, x + (size - width) / 2, y + (size - height) / 2, width, height
				, PlayerHUDHeadlessSrcX, PlayerHUDHeadlessSrcY, PlayerHUDHeadlessSrcWidth, PlayerHUDHeadlessSrcHeight);
		}

		// 교대 슬롯 (원작: 보조 스컬 작은 초상화 + SPACE). 두 번째 스컬 리소스가 없어 빈 회색 칸으로 둔다
		const int slotX = 10;
		const int slotY = 838;
		const int slotSize = 40;
		UIDraw::DrawBox(hdc, slotX, slotY, slotSize, slotSize, RGB(46, 46, 50), RGB(120, 120, 120), 2);
		UIDraw::DrawShade(hdc, slotX, slotY, slotSize, slotSize, RGB(0, 0, 0), 110);

		UIDraw::TextStyle style;
		style.size = 11;
		style.bold = true;
		style.color = RGB(150, 150, 150);
		style.align = UIDraw::eAlign::Center;
		style.shadow = true;
		UIDraw::DrawLabel(hdc, slotX + slotSize / 2, slotY + slotSize + 2, L"SPACE", style);
	}

	void PlayerHUD::renderSkill(HDC hdc, int x, int y, const wchar_t* key, bool skillA)
	{
		const int size = PlayerHUDSkillSize;
		UIDraw::DrawBox(hdc, x, y, size, size, PlayerHUDFrameFill, PlayerHUDFrameBorder, 2);

		// 아이콘: A는 던지는 머리, S는 머리로 옮겨 갈 몸
		if (skillA)
		{
			const float scale = 2.6f;
			int width = (int)(mHeadImage->GetWidth() * scale);
			int height = (int)(mHeadImage->GetHeight() * scale);
			UIDraw::DrawImage(hdc, mHeadImage, x + (size - width) / 2, y + (size - height) / 2 + 2, scale);
		}
		else
		{
			int width = (int)(PlayerHUDHeadlessSrcWidth * 1.3f);
			int height = (int)(PlayerHUDHeadlessSrcHeight * 1.3f);
			UIDraw::DrawImagePart(hdc, mHeadlessSheet, x + (size - width) / 2, y + (size - height) / 2 + 4, width, height
				, PlayerHUDHeadlessSrcX, PlayerHUDHeadlessSrcY, PlayerHUDHeadlessSrcWidth, PlayerHUDHeadlessSrcHeight);
		}

		// 쿨다운이 돌고 있으면 남은 초, 쿨다운은 없지만 지금 못 쓰면 회색
		// (A: 머리가 나가 있음, S: 머리가 붙어 있음)
		float remain = skillA ? PlayerStats::GetSkillACooldown() : PlayerStats::GetSkillSCooldown();
		float total = skillA ? PlayerStats::SkillACooldownTime : PlayerStats::SkillSCooldownTime;
		bool usable = skillA ? !PlayerStats::IsHeadOut() : PlayerStats::IsHeadOut();
		if (remain > 0.0f)
			UIDraw::DrawCooldown(hdc, x, y, size, size, remain / total, remain);
		else if (!usable)
			UIDraw::DrawShade(hdc, x, y, size, size, RGB(0, 0, 0), 160);

		UIDraw::TextStyle style;
		style.size = 14;
		style.bold = true;
		style.color = RGB(255, 236, 180);
		style.shadow = true;
		UIDraw::DrawLabel(hdc, x + 4, y + 2, key, style);
	}

	void PlayerHUD::renderHPBar(HDC hdc)
	{
		int hp = PlayerStats::GetHP();
		int maxHP = PlayerStats::GetMaxHP();
		float maxValue = maxHP > 0 ? (float)maxHP : 1.0f;

		UIDraw::BarStyle barStyle;
		UIDraw::DrawBar(hdc, PlayerHUDBarX, PlayerHUDBarY, PlayerHUDBarWidth, PlayerHUDBarHeight
			, hp / maxValue, barStyle, mTrailHP / maxValue);

		wchar_t text[32] = {};
		swprintf_s(text, L"%d / %d", hp, maxHP);

		UIDraw::TextStyle style;
		style.size = 16;
		style.bold = true;
		style.align = UIDraw::eAlign::Center;
		style.shadow = true;
		UIDraw::DrawLabelBox(hdc, PlayerHUDBarX, PlayerHUDBarY, PlayerHUDBarWidth, PlayerHUDBarHeight, text, style);
	}

	void PlayerHUD::renderDamageFlash(HDC hdc)
	{
		if (mDamageFlash <= 0.0f)
			return;

		// 화면 네 가장자리를 붉게 칠하고 점점 옅어진다
		BYTE alpha = (BYTE)(120.0f * (mDamageFlash / PlayerHUDDamageFlashTime));
		const COLORREF color = RGB(200, 20, 20);
		const int width = (int)application.GetWidth();
		const int height = (int)application.GetHeight();
		const int edge = PlayerHUDDamageFlashWidth;
		UIDraw::DrawShade(hdc, 0, 0, width, edge, color, alpha);
		UIDraw::DrawShade(hdc, 0, height - edge, width, edge, color, alpha);
		UIDraw::DrawShade(hdc, 0, edge, edge, height - edge * 2, color, alpha);
		UIDraw::DrawShade(hdc, width - edge, edge, edge, height - edge * 2, color, alpha);
	}
}
