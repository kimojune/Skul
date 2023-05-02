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
	Boss_Hand::Boss_Hand()
		:mDirection(eDirection::Left)
	{
	}
	Boss_Hand::Boss_Hand(bool isRight)
	{
		mDirection = (eDirection)isRight;
	}
	Boss_Hand::~Boss_Hand()
	{
	}
	void Boss_Hand::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.5f, 2.5f));
	
		Vector2 handpos = tr->GetPos();

		handpos.y += 200.0f;
		
		if(mDirection==eDirection::Left)
			handpos.x -= 250.0f;
		else
			handpos.x += 250.0f;

		tr->SetPos(handpos);

		mPrevPos = handpos;


		mLeftImage[0] = Resources::Load<Image>(L"Boss_LeftHand", L"..\\Resources\\Boss\\Hand\\Boss_Left_Hand.bmp");
		mLeftImage[1] = Resources::Load<Image>(L"Boss_LeftPunch", L"..\\Resources\\Boss\\Hand\\Punch_1.bmp");
		mLeftImage[2] = Resources::Load<Image>(L"Boss_LeftPutOn", L"..\\Resources\\Boss\\Hand\\PutOn_1.bmp");
		mLeftImage[3] = Resources::Load<Image>(L"Boss_LeftSmash", L"..\\Resources\\Boss\\Hand\\Smesh_1.bmp");

		mRightImage[0] = Resources::Load<Image>(L"Boss_RightHand", L"..\\Resources\\Boss\\Hand\\Boss_Right_Hand.bmp");
		mRightImage[1] = Resources::Load<Image>(L"Boss_RightPunch", L"..\\Resources\\Boss\\Hand\\Punch_1_Right.bmp");
		mRightImage[2] = Resources::Load<Image>(L"Boss_RightPutOn", L"..\\Resources\\Boss\\Hand\\PutOn_1_Right.bmp");
		mRightImage[3] = Resources::Load<Image>(L"Boss_RightSmash", L"..\\Resources\\Boss\\Hand\\Smesh_1_Right.bmp");



		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimation(L"Boss_LeftHand", mLeftImage[0], Vector2(0, 0), 1, 5, 1, 5, Vector2(-84.0f,-76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_LeftPunch", mLeftImage[1], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f,-76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_LeftPutOn", mLeftImage[2], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f,-76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_LeftSmash", mLeftImage[3], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f,-76.0f), 0.3);

		mAnimator->GetStartEvent(L"Boss_LeftHand") = std::bind(&Boss_Hand::StartIdle, this);
		mAnimator->GetCompleteEvent(L"Boss_LeftHand") = std::bind(&Boss_Hand::CompleteIdle, this);
		mAnimator->GetEndEvent(L"Boss_LeftHand") = std::bind(&Boss_Hand::EndIdle, this);

		mAnimator->GetStartEvent(L"Boss_LeftPutOn") = std::bind(&Boss_Hand::StartDown, this);
		mAnimator->GetCompleteEvent(L"Boss_LeftPutOn") = std::bind(&Boss_Hand::CompleteDown, this);
		mAnimator->GetEndEvent(L"Boss_LeftPutOn") = std::bind(&Boss_Hand::EndDown, this);
		//mAnimator->GetCompleteEvent(L"LeftSkillA") = std::bind(&Skul::CompleteSkillA, this);
		//mAnimator->GetEndEvent(L"LeftSkillA") = std::bind(&Skul::EndSkillA, this);

		mAnimator->GetStartEvent(L"Boss_LeftPunch") = std::bind(&Boss_Hand::StartPunch, this);
		mAnimator->GetCompleteEvent(L"Boss_LeftPunch") = std::bind(&Boss_Hand::CompletePunch, this);
		mAnimator->GetEndEvent(L"Boss_LeftPunch") = std::bind(&Boss_Hand::EndPunch, this);

		mAnimator->GetStartEvent(L"Boss_LeftSmash") = std::bind(&Boss_Hand::StartSmash, this);
		mAnimator->GetCompleteEvent(L"Boss_LeftSmash") = std::bind(&Boss_Hand::CompleteSmash, this);
		mAnimator->GetEndEvent(L"Boss_LeftSmash") = std::bind(&Boss_Hand::EndSmash, this);


		mAnimator->CreateAnimation(L"Boss_RightHand", mRightImage[0], Vector2(672.0f, 0.0f), -1, 5, 1, 5, Vector2(-84.0f, -76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_RightPutOn", mRightImage[1], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_RightSmesh", mRightImage[2], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_RightPunch", mRightImage[3], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);


		mAnimator->GetStartEvent(L"Boss_RightPutOn") = std::bind(&Boss_Hand::StartDown, this);


		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(250.0f, 250.0f));
		collider->SetCenter(Vector2(-50.0f, -125.0f));

		mHandState = eHandState::Idle;
		mAnimator->Play(L"Boss_LeftHand", true);
		mPlayed = true;


		Scene* scene = SceneManager::GetActiveScene();
		mBoss =dynamic_cast<Chapter1_Boss*>(scene->GetGameObjects(eLayerType::Monster)[0]);

		GameObject::Initialize();
		
	}
	void Boss_Hand::Update()
	{
		switch (mHandState)
		{
		case ya::Boss_Hand::eHandState::Idle:
			Idle();
			break;
		case ya::Boss_Hand::eHandState::Down:
			Down();
			break;
		case ya::Boss_Hand::eHandState::Punch:
			Punch();
			break;
		case ya::Boss_Hand::eHandState::Smash:
			Smash();
			break;
		default:
			break;
		}

		GameObject::Update();

	}
	void Boss_Hand::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void Boss_Hand::Release()
	{
		GameObject::Release();
	}

	void Boss_Hand::Idle()
	{
		if (mPlayed == false)
		{
			mPlayed = true;
			mAnimator->Play(L"Boss_LeftHand", true);
		}

	}
	void Boss_Hand::Down()
	{

		if (!mPlayed)
		{
			mPlayed = true;
			mAnimator->Play(L"Boss_LeftPutOn", false);
		}
	}

	void Boss_Hand::Punch()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 handpos = tr->GetPos();

		if (mPlayed == false)
		{
			mPlayed = true;
			mAnimator->Play(L"Boss_LeftPunch", false);
		}
		

		handpos += (mTargetPos - handpos).Normalize() * 300 * Time::DeltaTime();
		tr->SetPos(handpos);

		if (handpos == mTargetPos)
		{
			mHandState = eHandState::Idle;
			mAnimator->Play(L"Boss_LeftHand", true);
			mComplete = true;
		}
	}
	void Boss_Hand::Smash()
	{
		if (mPlayed == false)
		{
			mPlayed = true;
			mAnimator->Play(L"Boss_LeftPutOn", false);
		}
		Transform* tr = GetComponent<Transform>();
		Vector2 handpos = tr->GetPos();
		handpos.x = 0.0f;
		
	}

	void Boss_Hand::StartIdle()
	{
		mComplete = false;

	}

	void Boss_Hand::CompleteIdle()
	{

	}

	void Boss_Hand::EndIdle()
	{

		mComplete = true;
	}

	void Boss_Hand::StartDown()
	{
		Transform* handtr = GetComponent<Transform>();
		Vector2 handpos = handtr->GetPos();
		handpos.y += 200.0f;
		handtr->SetPos(handpos);
	}
	void Boss_Hand::CompleteDown()
	{

	}
	void Boss_Hand::EndDown()
	{
		Transform* handtr = GetComponent<Transform>();
		handtr->SetPos(mPrevPos);
		mComplete = true;

	}
	void Boss_Hand::StartPunch()
	{/*
		mHandState = eHandState::Punch;
		PlayeScene* playscene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
		Skul* skul = playscene->GetSkul();
		if (skul == nullptr)
			return;
		Transform* skultr = skul->GetComponent<Transform>();

		mTargetPos = skultr->GetPos();*/
	}
	void Boss_Hand::CompletePunch()
	{
	}
	void Boss_Hand::EndPunch()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetPos(mPrevPos);
	}

	void Boss_Hand::StartSmash()
	{
	}

	void Boss_Hand::CompleteSmash()
	{
	}

	void Boss_Hand::EndSmash()
	{
		mComplete = true;
	}
}