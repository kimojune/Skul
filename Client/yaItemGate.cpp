#include "yaItemGate.h"
#include "yaAnimator.h"
namespace ya
{
	ItemGate::ItemGate()
	{
	}
	ItemGate::~ItemGate()
	{
	}
	void ItemGate::Initialize()
	{
		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimations(L"..\\Resources\\Object\\Gate\\ItemGate\\DeActivate", Vector2(-14.0f, -8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Object\\Gate\\ItemGate\\Activate", Vector2(-14.0f, -8.0f), 0.1f);

		mAnimator->Play(L"ItemDeActivate", false);

		GameObject::Initialize();
	}
	void ItemGate::Update()
	{
		GameObject::Update();
	}
	void ItemGate::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void ItemGate::Release()
	{
		GameObject::Release();
	}
}