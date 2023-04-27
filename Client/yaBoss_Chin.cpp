#include "yaBoss_Chin.h"
#include "yaAnimator.h"
#include "yaResources.h"
#include "yaTransform.h"
#include "yaTime.h"

namespace ya
{
	Boss_Chin::Boss_Chin()
		:mTime(0.0f)
	{
	}
	Boss_Chin::~Boss_Chin()
	{
	}
	void Boss_Chin::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.5f, 2.5f));
		Vector2 chinPos = tr->GetPos();

		chinPos.x += 250.0f;
		chinPos.y += 200.0f;
		tr->SetPos(chinPos);

		
		mImage[0] = Resources::Load<Image>(L"Boss_Chin_1", L"..\\Resources\\Boss\\Chin\\Chin_1.bmp");
		mImage[1] = Resources::Load<Image>(L"Boss_Chin_2", L"..\\Resources\\Boss\\Chin\\Chin_2.bmp");
		mImage[2] = Resources::Load<Image>(L"Boss_Chin_3", L"..\\Resources\\Boss\\Chin\\Chin_3.bmp");

		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimation(L"BossChin_1", mImage[0], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"BossChin_2", mImage[1], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"BossChin_3", mImage[2], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);

		mAnimator->Play(L"BossChin_1", false);


		GameObject::Initialize();
	}
	void Boss_Chin::Update()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 chinPos = tr->GetPos();

		mTime += Time::DeltaTime();

		if (mTime >= 0.2)
		{
			chinPos.y+= std::sin(chinPos.y) * 10;
			tr->SetPos(chinPos);
			mTime = 0;
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
	void Boss_Chin::Idle()
	{
	}
	void Boss_Chin::Open()
	{
	}
	void Boss_Chin::Close()
	{
	}
}