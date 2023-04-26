#include "yaBossBullet.h"
#include "yaAnimator.h"

namespace ya
{
	BossBullet::BossBullet()
	{
	}
	BossBullet::~BossBullet()
	{
	}
	void BossBullet::Initialize()
	{
		mAnimator = AddComponent<Animator>();
		mAnimator->CreateAnimations(L"..\\Resources\\Boss\\Effect\\Bullet\\Smoke", Vector2(-14.0f, -8.0f), 0.1f);


		mAnimator->Play(L"BulletSmoke", true);


		GameObject::Initialize();

	}
	void BossBullet::Update()
	{
		GameObject::Update();

	}
	void BossBullet::Render(HDC hdc)
	{
		GameObject::Render(hdc);

	}
	void BossBullet::Release()
	{
		GameObject::Release();

	}
	void BossBullet::OnCollisionEnter(Collider* other)
	{
	}
	void BossBullet::OnCollisionStay(Collider* other)
	{
	}
	void BossBullet::OnCollisionExit(Collider* other)
	{
	}
}