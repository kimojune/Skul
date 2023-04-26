#include "yaBoss_Body.h"
#include "yaAnimator.h"
#include "yaResources.h"
#include "yaTransform.h"

namespace ya
{
	Boss_Body::Boss_Body()
	{
	}
	Boss_Body::~Boss_Body()
	{
	}
	void Boss_Body::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.5f, 2.5f));
		Vector2 bosspos = tr->GetPos();
		

		mImage[0] = Resources::Load<Image>(L"Boss_Body_1", L"..\\Resources\\Boss\\Body\\Boss_Body_1.bmp");
		mImage[1] = Resources::Load<Image>(L"Boss_Body_2", L"..\\Resources\\Boss\\Body\\Boss_Body_2.bmp");
		mImage[2] = Resources::Load<Image>(L"Boss_Body_3", L"..\\Resources\\Boss\\Body\\Boss_Body_3.bmp");
		
		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimation(L"BossBody_1", mImage[0], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"BossBody_2", mImage[1], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"BossBody_3", mImage[2], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);

		mAnimator->Play(L"BossBody_1", false);

		
		GameObject::Initialize();

	}
	void Boss_Body::Update()
	{
		GameObject::Update();

	}
	void Boss_Body::Render(HDC hdc)
	{
		GameObject::Render(hdc);

	}
	void Boss_Body::Release()
	{
	}
}