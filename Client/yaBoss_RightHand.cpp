#include "yaBoss_RightHand.h"
#include "yaBoss_Hand.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaResources.h"
#include "yaSceneManager.h"
#include "yaPlayeScene.h"
#include "yaSkul.h"
#include "yaTime.h"
#include "yaCollider.h"
#include "yaChapter1_Boss.h"

namespace ya
{
	Boss_RightHand::Boss_RightHand()
		:mDirection(eDirection::Right)
	{
	}

	Boss_RightHand::~Boss_RightHand()
	{
	}
	void Boss_RightHand::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.5f, 2.5f));

		Vector2 handpos = tr->GetPos();

		handpos.y += 100.0f;

		if (mDirection == eDirection::Left)
			handpos.x -= 250.0f;
		else
			handpos.x += 650.0f;

		tr->SetPos(handpos);

		mPrevPos = handpos;


		mRightImage[0] = Resources::Load<Image>(L"Boss_RightHand", L"..\\Resources\\Boss\\Hand\\Boss_Right_Hand.bmp");
		mRightImage[1] = Resources::Load<Image>(L"Boss_RightPunch", L"..\\Resources\\Boss\\Hand\\Punch_1_Right.bmp");
		mRightImage[2] = Resources::Load<Image>(L"Boss_RightPutOn", L"..\\Resources\\Boss\\Hand\\PutOn_1_Right.bmp");
		mRightImage[3] = Resources::Load<Image>(L"Boss_RightSmash", L"..\\Resources\\Boss\\Hand\\Smesh_1_Right.bmp");


		mAnimator = AddComponent<Animator>();


		mAnimator->CreateAnimation(L"Boss_RightHand", mRightImage[0], Vector2(672.0f, 0.0f), -1, 5, 1, 5, Vector2(-84.0f, -76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_RightPunch", mRightImage[1], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_RightPutOn", mRightImage[2], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_RightSmash", mRightImage[3], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);



		mAnimator->GetStartEvent(L"Boss_RightHand") = std::bind(&Boss_RightHand::StartIdle, this);
		mAnimator->GetCompleteEvent(L"Boss_RightHand") = std::bind(&Boss_RightHand::CompleteIdle, this);
		mAnimator->GetEndEvent(L"Boss_RightHand") = std::bind(&Boss_RightHand::EndIdle, this);

		mAnimator->GetStartEvent(L"Boss_RightPutOn") = std::bind(&Boss_RightHand::StartDown, this);
		mAnimator->GetCompleteEvent(L"Boss_RightPutOn") = std::bind(&Boss_RightHand::CompleteDown, this);
		mAnimator->GetEndEvent(L"Boss_RightPutOn") = std::bind(&Boss_RightHand::EndDown, this);
		//mAnimator->GetCompleteEvent(L"LeftSkillA") = std::bind(&Skul::CompleteSkillA, this);
		//mAnimator->GetEndEvent(L"LeftSkillA") = std::bind(&Skul::EndSkillA, this);

		mAnimator->GetStartEvent(L"Boss_RightPunch") = std::bind(&Boss_RightHand::StartPunch, this);
		mAnimator->GetCompleteEvent(L"Boss_RightPunch") = std::bind(&Boss_RightHand::CompletePunch, this);
		mAnimator->GetEndEvent(L"Boss_RightPunch") = std::bind(&Boss_RightHand::EndPunch, this);

		mAnimator->GetStartEvent(L"Boss_RightSmash") = std::bind(&Boss_RightHand::StartSmash, this);
		mAnimator->GetCompleteEvent(L"Boss_RightSmash") = std::bind(&Boss_RightHand::CompleteSmash, this);
		mAnimator->GetEndEvent(L"Boss_RightSmash") = std::bind(&Boss_RightHand::EndSmash, this);


		//mAnimator->GetStartEvent(L"Boss_RightPutOn") = std::bind(&Boss_Hand::StartDown, this);


		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(250.0f, 250.0f));
		collider->SetCenter(Vector2(-50.0f, -125.0f));

		mHandState = eHandState::Down;
		mAnimator->Play(L"Boss_RightPutOn", false);
		mPlayed = true;
		mTime = 0;
		//mAnimator->CreateAnimation(L"Boss_RightHand", mRightImage[0], Vector2(672.0f, 0.0f), -1, 5, 1, 5, Vector2(-84.0f, -76.0f), 0.3);
		//mAnimator->CreateAnimation(L"Boss_RightPunch", mRightImage[1], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);
		//mAnimator->CreateAnimation(L"Boss_RightPutOn", mRightImage[2], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);
		//mAnimator->CreateAnimation(L"Boss_RightSmesh", mRightImage[3], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);


		//mAnimator->GetStartEvent(L"Boss_RightPutOn") = std::bind(&Boss_RightHand::StartDown, this);

		//if (mDirection == eDirection::Left)
		//	mAnimator->Play(L"Boss_RightHand", true);
		//else
		//	mAnimator->Play(L"Boss_RightHand", true);


		//Collider* collider = AddComponent<Collider>();
		//collider->SetSize(Vector2(250.0f, 250.0f));
		//collider->SetCenter(Vector2(-50.0f, -125.0f));

		//mHandState = eHandState::Down;
		//mAnimator->Play(L"Boss_RightPutOn", true);

		//Scene* scene = SceneManager::GetActiveScene();
		//mBoss = dynamic_cast<Chapter1_Boss*>(scene->GetGameObjects(eLayerType::Monster)[0]);

		//GameObject::Initialize();

	}
	void Boss_RightHand::Update()
	{

		switch (mHandState)
		{
		case ya::Boss_RightHand::eHandState::Idle:
			Idle();
			break;
		case ya::Boss_RightHand::eHandState::Down:
			Down();
			break;
		case ya::Boss_RightHand::eHandState::Punch:
			Punch();
			break;
		case ya::Boss_RightHand::eHandState::Smash:
			Smash();
			break;
		default:
			break;
		}

		GameObject::Update();

	}
	void Boss_RightHand::Render(HDC hdc)
	{
		GameObject::Render(hdc);

	}
	void Boss_RightHand::Release()
	{
		GameObject::Release();

	}

	void Boss_RightHand::Idle()
	{
		if (mPlayed == false)
		{
			mPlayed = true;
			mAnimator->Play(L"Boss_RightHand", true);
		}
	}
	void Boss_RightHand::Down()
	{
		if (!mPlayed)
		{
			mPlayed = true;
			mAnimator->Play(L"Boss_RightPutOn", false);
		}
	}

	void Boss_RightHand::Punch()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 handpos = tr->GetPos();

		if (mPlayed == false)
		{
			mPlayed = true;
			mComplete = false;
			mAnimator->Play(L"Boss_RightPunch", false);
			handpos.y -= 200.0f;

			tr->SetPos(handpos);
		}

		mTime += Time::DeltaTime();

		if (mTime > 1.0f)
		{
			if ((mTargetPos - handpos).Length() > 50.0f)
			{
				handpos += (mTargetPos - handpos).Normalize() * 2000 * Time::DeltaTime();
				tr->SetPos(handpos);
			}
			else
				mComplete = true;
		}

	}
	void Boss_RightHand::Smash()
	{
		if (mPlayed == false)
		{
			mPlayed = true;
			mAnimator->Play(L"Boss_RightSmash", false);
		}

		Transform* handtr = GetComponent<Transform>();
		Vector2 handpos = handtr->GetPos();

		mTime += Time::DeltaTime();

		if (mTime > 1)
		{
			if (handpos.x > 0)
			{
				handpos.x -= 3000 * Time::DeltaTime();
				handtr->SetPos(handpos);

			}
			else
				mComplete = true;
		}
	}

	void Boss_RightHand::StartIdle()
	{
		mComplete = false;
	}

	void Boss_RightHand::CompleteIdle()
	{

	}

	void Boss_RightHand::EndIdle()
	{

	}

	void Boss_RightHand::StartDown()
	{
		Transform* handtr = GetComponent<Transform>();
		Vector2 handpos = handtr->GetPos();
		handpos.y += 220.0f;
		handtr->SetPos(handpos);
	}
	void Boss_RightHand::CompleteDown()
	{

	}
	void Boss_RightHand::EndDown()
	{
		Transform* handtr = GetComponent<Transform>();
		handtr->SetPos(mPrevPos);
		mComplete = true;

	}
	void Boss_RightHand::StartPunch()
	{

	}
	void Boss_RightHand::CompletePunch()
	{
	}
	void Boss_RightHand::EndPunch()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetPos(mPrevPos);
		mTime = 0;
	}

	void Boss_RightHand::StartSmash()
	{
		Transform* handtr = GetComponent<Transform>();
		handtr->SetPos(Vector2(3700, 1700));
	}

	void Boss_RightHand::CompleteSmash()
	{
	}

	void Boss_RightHand::EndSmash()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetPos(mPrevPos);
		mTime = 0;
	}
}