#include "yaBossEffect.h"
#include "yaAnimator.h"
#include "yaResources.h"
#include "yaImage.h"
#include "yaTransform.h"
#include "yaTime.h"
#include "yaCamera.h"
#include "yaUIDraw.h"

namespace ya
{
	// 이펙트 종류별 폴더, 배율, 프레임 시간, 그 폴더의 가장 큰 프레임 크기(Animator가 칸 크기로 쓴다).
	// 배율을 키우면 Animator가 왼쪽 위부터 늘려 그리므로, 늘린 그림의 아래쪽 가운데가 위치에 오도록 offset을 준다.
	// 32비트 원본(엔진의 LoadImageW가 못 읽음)은 각 폴더의 rgba32 하위 폴더로 옮겨 두었다.
	// CreateAnimations는 하위 폴더를 읽지 않으므로 마젠타 배경판 24비트 그림만 쓰인다.
	struct BossEffectInfo
	{
		const wchar_t* path;
		const wchar_t* name;	// CreateAnimations가 붙이는 이름 = 상위 폴더명 + 폴더명
		float scale;
		float duration;
		Vector2 cell;
	};
	static const BossEffectInfo BossEffectInfos[(UINT)BossEffect::eType::End] =
	{
		{ L"..\\Resources\\Boss\\Effect\\Appearance Impact", L"EffectAppearance Impact", 1.3f, 0.05f, Vector2(399.0f, 189.0f) },
		{ L"..\\Resources\\Boss\\Effect\\smoke", L"Effectsmoke", 1.5f, 0.033f, Vector2(548.0f, 142.0f) },
		{ L"..\\Resources\\Boss\\Effect\\Bullet\\Smoke", L"BulletSmoke", 1.8f, 0.033f, Vector2(344.0f, 185.0f) },
	};

	static Vector2 BossEffectOffset(Vector2 cell, Vector2 scale)
	{
		return Vector2(-cell.x / 2.0f * (scale.x - 1.0f), -cell.y * (scale.y - 1.0f));
	}

	BossEffect::BossEffect()
		: mAnimator(nullptr)
	{
	}
	BossEffect::~BossEffect()
	{
	}
	void BossEffect::Initialize()
	{
		mAnimator = AddComponent<Animator>();

		for (const BossEffectInfo& info : BossEffectInfos)
		{
			mAnimator->CreateAnimations(info.path, BossEffectOffset(info.cell, Vector2(info.scale, info.scale)), info.duration);
			mAnimator->GetCompleteEvent(info.name) = std::bind(&BossEffect::complete, this);
		}

		GameObject::Initialize();
		SetState(eState::Pause);
	}
	void BossEffect::Update()
	{
		GameObject::Update();
	}
	void BossEffect::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void BossEffect::Release()
	{
		GameObject::Release();
	}

	void BossEffect::Play(eType type, Vector2 bottomCenter)
	{
		if (type >= eType::End)
			return;

		const BossEffectInfo& info = BossEffectInfos[(UINT)type];
		Transform* tr = GetComponent<Transform>();
		tr->SetPos(bottomCenter);
		tr->SetScale(Vector2(info.scale, info.scale));

		SetState(eState::Active);
		mAnimator->Play(info.name, false);
	}

	void BossEffect::complete()
	{
		SetState(eState::Pause);
	}

	// ---------------------------------------------------------------------------------------------

	// Range 폴더: Loop_0, Start_0~4 (이름순으로 Loop가 먼저). 가장 큰 프레임 36x99.
	// E3가 PNG를 변환할 때 반투명한 빛 번짐이 빠져서 바닥 쪽 밝은 줄만 남아 있다.
	static const Vector2 BossTelegraphCell = Vector2(36.0f, 99.0f);
	static const float BossTelegraphFloorY = 1752.0f;
	static const float BossTelegraphScaleY = 2.0f;

	BossTelegraph::BossTelegraph()
		: mAnimator(nullptr)
		, mCenterX(0.0f)
		, mTime(0.0f)
	{
	}
	BossTelegraph::~BossTelegraph()
	{
	}
	void BossTelegraph::Initialize()
	{
		Vector2 scale = Vector2(Width / BossTelegraphCell.x, BossTelegraphScaleY);
		GetComponent<Transform>()->SetScale(scale);
		Vector2 offset = BossEffectOffset(BossTelegraphCell, scale);

		mAnimator = AddComponent<Animator>();
		const std::wstring path = L"..\\Resources\\Effect\\Range";
		mAnimator->CreateAnimations(path, offset, 0.06f);
		Image* sheet = Resources::Find<Image>(path);
		if (sheet != nullptr)
		{
			// 6칸 중 1~5칸(Start_0~4)만. 반복하지 않으므로 끝나면 마지막 칸(가장 밝음)에 머문다
			mAnimator->CreateAnimation(L"TelegraphStart", sheet, Vector2(BossTelegraphCell.x, 0.0f), 1, 6, 1, 5, offset, 0.06f);
		}

		GameObject::Initialize();
		SetState(eState::Pause);
	}
	void BossTelegraph::Update()
	{
		mTime += (float)Time::DeltaTime();
		GameObject::Update();
	}
	void BossTelegraph::Render(HDC hdc)
	{
		// 바닥에서 위로 갈수록 옅어지는 붉은 빛. 깜빡이며 세진다
		float pulse = 0.75f + 0.25f * sinf(mTime * 14.0f);
		float grow = mTime < 0.25f ? mTime / 0.25f : 1.0f;
		Vector2 leftBottom = Camera::CaluatePos(Vector2(mCenterX - Width / 2.0f, BossTelegraphFloorY));

		// 띠 12개(240px). 위로 갈수록 제곱으로 옅어져서 윗변 경계가 드러나지 않는다
		const int bands = 12;
		const int bandHeight = 20;
		for (int i = 0; i < bands; i++)
		{
			float fade = 1.0f - (float)i / (float)bands;
			float alpha = 140.0f * pulse * grow * fade * fade;
			UIDraw::DrawShade(hdc, (int)leftBottom.x, (int)leftBottom.y - bandHeight * (i + 1)
				, (int)Width, bandHeight, RGB(255, 30, 70), (BYTE)alpha);
		}

		GameObject::Render(hdc);
	}
	void BossTelegraph::Release()
	{
		GameObject::Release();
	}

	void BossTelegraph::Show(float centerX)
	{
		mCenterX = centerX;
		mTime = 0.0f;
		GetComponent<Transform>()->SetPos(Vector2(centerX, BossTelegraphFloorY + 12.0f));
		SetState(eState::Active);
		mAnimator->Play(L"TelegraphStart", false);
	}

	void BossTelegraph::Hide()
	{
		SetState(eState::Pause);
	}
}
