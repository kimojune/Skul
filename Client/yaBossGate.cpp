#include "yaBossGate.h"
#include "yaAnimator.h"

namespace ya
{
	BossGate::BossGate()
	{
	}
	BossGate::~BossGate()
	{
	}
	void BossGate::Initialize()
	{
		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimations(L"..\\Resources\\Object\\Gate\\BossGate\\DeActivate", Vector2(-14.0f, -8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Object\\Gate\\BossGate\\Activate", Vector2(-14.0f, -8.0f), 0.1f);

		Gate::Initialize();
	}
	void BossGate::Update()
	{
		Gate::Update();

	}
	void BossGate::Render(HDC hdc)
	{
		Gate::Render(hdc);

	}
	void BossGate::Release()
	{
		Gate::Release();

	}
}
