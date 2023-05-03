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
		
		mTime = 0;
		mPrevPos = headPos;
		mUpPos = Vector2(headPos.x, headPos.y);
		mDownPos = Vector2(headPos.x, headPos.y + 1000);
		mLeftPos = Vector2(headPos.x - 600, headPos.y);
		mRightPos = Vector2(headPos.x + 600, headPos.y);

		tr->SetPos(mDownPos);
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

		tr->SetPos(mDownPos);
		mHeadState = eHeadState::UP;
		mPlayed = false;

		GameObject::Initialize();
	}
	void Boss_Head::Update()
	{
		switch (mHeadState)
		{
		case ya::Boss_Head::eHeadState::Idle:
			Idle();
			break;
		case ya::Boss_Head::eHeadState::UP:
			UP();
			break;
		case ya::Boss_Head::eHeadState::Down:
			Down();
			break;
		case ya::Boss_Head::eHeadState::Left:
			Left();
			break;
		case ya::Boss_Head::eHeadState::Right:
			Right();
			break;
		default:
			break;
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
	void Boss_Head::Idle()
	{
		Transform* tr = GetComponent<Transform>();
		if (!(mPlayed))
		{
			tr->SetPos(mPrevPos);
			mPlayed = true;
			mTime = 0;

		}

		Vector2 chinPos = tr->GetPos();

		mTime += Time::DeltaTime();

		chinPos.y += std::sin(chinPos.y) * 5;

		if (mTime >= 0.3)
		{
			tr->SetPos(chinPos);
			mTime = 0;
		}
	}
	void Boss_Head::UP()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 headpos = tr->GetPos();

		if (headpos.y <= mPrevPos.y)
		{
			headpos = mPrevPos;
			mHeadState = eHeadState::Idle;
		}
		else
		{
			headpos.y -= 500 * Time::DeltaTime();
		}

		tr->SetPos(headpos);

	}
	void Boss_Head::Down()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 bodypos = tr->GetPos();

		if (bodypos.y >= mDownPos.y)
		{
			bodypos = mPrevPos;
			mHeadState = eHeadState::Idle;
		}
		else
		{
			bodypos.y += 300 * Time::DeltaTime();
		}

		tr->SetPos(bodypos);
	}
	void Boss_Head::Left()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 bodypos = tr->GetPos();

		if ((!mPlayed))
		{
			bodypos = mLeftPos;
			mPlayed = true;
			tr->SetPos(bodypos);

		}

		mTime += Time::DeltaTime();

		if (mTime > 3.0)
		{
			if (bodypos.x >= mRightPos.x)
			{
				bodypos = mRightPos;
				mTime = 0;
			}
			else
				bodypos.x += 1000 * Time::DeltaTime();
		
			tr->SetPos(bodypos);
		}

	}
	void Boss_Head::Right()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 bodypos = tr->GetPos();

		if ((!mPlayed))
		{
			bodypos = mRightPos;
			mPlayed = true;
			tr->SetPos(bodypos);

		}

		mTime += Time::DeltaTime();

		if (mTime > 3.0)
		{
			if (bodypos.x <= mLeftPos.x)
			{
				bodypos = mLeftPos;
				mTime = 0;
			}
			else
				bodypos.x -= 2000 * Time::DeltaTime();
			
			tr->SetPos(bodypos);
		}

	}

}