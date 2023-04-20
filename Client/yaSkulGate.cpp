#include "yaSkulGate.h"
#include "yaAnimator.h"

namespace ya
{
	SkulGate::SkulGate()
	{
	}
	SkulGate::~SkulGate()
	{
	}
	void SkulGate::Initialize()
	{
		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimations(L"..\\Resources\\Object\\Gate\\SkulGate\\DeActivate", Vector2(-14.0f, -8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Object\\Gate\\SkulGate\\Activate", Vector2(-14.0f, -8.0f), 0.1f);
		mAnimator->Play(L"SkulDeActivate", false);

		GameObject::Initialize();
	}
	void SkulGate::Update()
	{
		GameObject::Update();
	}
	void SkulGate::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void SkulGate::Release()
	{
		GameObject::Release();
	}
}