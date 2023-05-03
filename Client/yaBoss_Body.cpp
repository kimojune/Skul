#include "yaBoss_Body.h"
#include "yaAnimator.h"
#include "yaResources.h"
#include "yaTransform.h"
#include "yaTime.h"

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
		
		mPrevPos = tr->GetPos();
		Vector2 bosspos = mPrevPos;
		
		

		mUpPos = Vector2(bosspos.x,bosspos.y);
		mDownPos = Vector2(bosspos.x, bosspos.y + 1000);
		mLeftPos = Vector2(bosspos.x - 300, bosspos.y);
		mRightPos = Vector2(bosspos.x + 300, bosspos.y);

		mImage[0] = Resources::Load<Image>(L"Boss_Body_1", L"..\\Resources\\Boss\\Body\\Boss_Body_1.bmp");
		mImage[1] = Resources::Load<Image>(L"Boss_Body_2", L"..\\Resources\\Boss\\Body\\Boss_Body_2.bmp");
		mImage[2] = Resources::Load<Image>(L"Boss_Body_3", L"..\\Resources\\Boss\\Body\\Boss_Body_3.bmp");
		
		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimation(L"BossBody_1", mImage[0], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"BossBody_2", mImage[1], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"BossBody_3", mImage[2], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1);

		mAnimator->Play(L"BossBody_1", false);

		tr->SetPos(mDownPos);
		mBodyState = eBodyState::UP;
		
		GameObject::Initialize();

	}
	void Boss_Body::Update()
	{
		switch (mBodyState)
		{
		case ya::Boss_Body::eBodyState::Idle:
			Idle();
			break;
		case ya::Boss_Body::eBodyState::UP:
			UP();
			break;
		case ya::Boss_Body::eBodyState::Down:
			Down();
			break;
		case ya::Boss_Body::eBodyState::Left:
			Left();
			break;
		case ya::Boss_Body::eBodyState::Right:
			Right();
			break;
		default:
			break;
		}
		GameObject::Update();

	}
	void Boss_Body::Render(HDC hdc)
	{
		GameObject::Render(hdc);

	}
	void Boss_Body::Release()
	{
	}
	void Boss_Body::Idle()
	{
		
	}
	void Boss_Body::UP()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 bodypos = tr->GetPos();

		if (bodypos.y <= mPrevPos.y)
		{
			bodypos = mPrevPos;
			mBodyState = eBodyState::Idle;
		}
		else
		{
			bodypos.y -= 500 * Time::DeltaTime();
		}
		
		tr->SetPos(bodypos);

	}
	void Boss_Body::Down()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 bodypos = tr->GetPos();

		if (bodypos.y >= mDownPos.y)
		{
			bodypos = mPrevPos;
			mBodyState = eBodyState::Idle;

		}
		else
		{
			bodypos.y += 300 * Time::DeltaTime();
		}

		tr->SetPos(bodypos);
	}
	void Boss_Body::Left()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 bodypos = tr->GetPos();

		if (bodypos.x <= mLeftPos.x)
		{
			bodypos = mLeftPos;
		}
		else
		{
			bodypos.x -= 800 * Time::DeltaTime();
		}

		tr->SetPos(bodypos);
	}
	void Boss_Body::Right()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 bodypos = tr->GetPos();

		if (bodypos.x >= mRightPos.x)
		{
			bodypos = mPrevPos;
		}
		else
		{
			bodypos.x += 800 * Time::DeltaTime();
		}

		tr->SetPos(bodypos);
	}
}