#include "yaBoss_Head.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaResources.h"
#include "yaCollider.h"
#include "yaTime.h"

namespace ya
{
	Boss_Head::Boss_Head()
	{
	}
	Boss_Head::~Boss_Head()
	{
	}
	void Boss_Head::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 headPos = tr->GetPos();
		
		headPos.x += 100.0f;
		tr->SetPos(headPos);

		tr->SetScale(Vector2(2.5f, 2.5f));

		mAnimator = AddComponent<Animator>();

		mImage[0] = Resources::Load<Image>(L"Boss_Head_1", L"..\\Resources\\Boss\\Head\\Boss_Head_1.bmp");
		mImage[1] = Resources::Load<Image>(L"Boss_Head_2", L"..\\Resources\\Boss\\Head\\Boss_Head_2.bmp");
		mImage[2] = Resources::Load<Image>(L"Boss_Head_3", L"..\\Resources\\Boss\\Head\\Boss_Head_3.bmp");

		mAnimator->CreateAnimation(L"BossHead_1", mImage[0], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"BossHead_2", mImage[1], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"BossHead_3", mImage[2], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);

		mAnimator->Play(L"BossHead_1", false);

		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(300.0f, 300.0f));
		collider->SetCenter(Vector2(-25.0f, -100.0f));

		GameObject::Initialize();
	}
	void Boss_Head::Update()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 chinPos = tr->GetPos();

		mTime += Time::DeltaTime();

		if (mTime >= 0.2)
		{
			chinPos.y += std::sin(chinPos.y) * 5;
			tr->SetPos(chinPos);
			mTime = 0;
		}

		GameObject::Update();

	}
	void Boss_Head::Render(HDC hdc)
	{
		GameObject::Render(hdc);

	}
	void Boss_Head::Release()
	{
	}
}