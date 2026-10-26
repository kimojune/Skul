#include "yaBoss_Chin.h"
#include "yaBoss_Head.h"
#include "yaAnimator.h"
#include "yaResources.h"
#include "yaTransform.h"
#include "yaTime.h"

namespace ya
{
	Boss_Chin::Boss_Chin()
		: mAnimator(nullptr)
		, mHead(nullptr)
		, mOpen(0.0f)
		, mOpened(false)
		, mbPhase2(false)
	{
	}
	Boss_Chin::~Boss_Chin()
	{
	}
	void Boss_Chin::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.5f, 2.5f));

		Image* image[2] = {};
		image[0] = Resources::Load<Image>(L"Boss_Chin_1", L"..\\Resources\\Boss\\Chin\\Chin_1.bmp");
		image[1] = Resources::Load<Image>(L"Boss_Chin_2", L"..\\Resources\\Boss\\Chin\\Chin_2.bmp");

		mAnimator = AddComponent<Animator>();
		mAnimator->CreateAnimation(L"BossChin_1", image[0], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1f);
		mAnimator->CreateAnimation(L"BossChin_2", image[1], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1f);
		mAnimator->Play(L"BossChin_1", false);

		GameObject::Initialize();
	}
	void Boss_Chin::Update()
	{
		// 입은 0.25초에 걸쳐 벌어지고 닫힌다
		float dt = (float)Time::DeltaTime();
		float target = mOpened ? 1.0f : 0.0f;
		if (mOpen < target)
			mOpen = (mOpen + dt * 4.0f < target) ? mOpen + dt * 4.0f : target;
		else if (mOpen > target)
			mOpen = (mOpen - dt * 4.0f > target) ? mOpen - dt * 4.0f : target;

		if (mHead != nullptr)
		{
			// 머리 기준 위치는 원래 값(닫힘 +250, 열림 +350)을 그대로 쓴다
			Vector2 pos = mHead->GetComponent<Transform>()->GetPos();
			pos.x += 150.0f;
			pos.y += 250.0f + 100.0f * mOpen;
			GetComponent<Transform>()->SetPos(pos);
		}

		GameObject::Update();
	}
	void Boss_Chin::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void Boss_Chin::Release()
	{
	}

	void Boss_Chin::SetPhase2(bool phase2)
	{
		if (mbPhase2 == phase2)
			return;

		mbPhase2 = phase2;
		mAnimator->Play(phase2 ? L"BossChin_2" : L"BossChin_1", false);
	}
}
