#pragma once


#include "yaBasicSkul.h"
#include "yaResources.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaSceneManager.h"
#include "yaPlayeScene.h"
#include "yaSkulHead.h"

namespace ya
{


	Basic::Basic()
		:mbPlay(false)
	{
	}
	Basic::~Basic()
	{
	}
	void Basic::Initialize()
	{
		SetName(L"BasicSkul");

		Image* LeftImage = Resources::Load<Image>(L"LeftSkul", L"..\\Resources\\SkulLeft.bmp");
		Image* RightImage = Resources::Load<Image>(L"RightSkul", L"..\\Resources\\SkulRight.bmp");

		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimation(L"LeftIdle", LeftImage, Vector2(2976.0f, 0.0f), -1, 25, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftRun", LeftImage, Vector2(2976.0f, 124.0f), -1, 25, 20, 8, Vector2(-85.0f, -50.0f), 0.08);
		mAnimator->CreateAnimation(L"LeftDash", LeftImage, Vector2(2976.0f, 124.0f * 2), -1, 25, 20, 1, Vector2(-85.0f, -50.0f), 1);
		mAnimator->CreateAnimation(L"LeftJump", LeftImage, Vector2(2976.0f, 124.0f * 3), -1, 25, 20, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftFall", LeftImage, Vector2(2976.0f, 124.0f * 4), -1, 25, 20, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftFallRepeat", LeftImage, Vector2(2976.0f, 124.0f * 5), -1, 25, 20, 3, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftAttackA", LeftImage, Vector2(2976.0f, 124.0f * 6), -1, 25, 20, 5, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftAttackB", LeftImage, Vector2(2976.0f, 124.0f * 7), -1, 25, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftJumpAttack", LeftImage, Vector2(2976.0f, 124.0f * 8), -1, 25, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftSkillA", LeftImage, Vector2(2976.0f, (124.0f * 9)), -1, 25, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftSkillS", LeftImage, Vector2(2976.0f, (124.0f * 12)), -1, 25, 20, 9, Vector2(-85.0f, -50.0f), 0.05);

		mAnimator->CreateAnimation(L"RightIdle", RightImage, Vector2(0.0f, 0.0f), 1, 25, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightRun", RightImage, Vector2(0.0f, 124.0f), 1, 25, 20, 8, Vector2(-85.0f, -50.0f), 0.08);
		mAnimator->CreateAnimation(L"RightDash", RightImage, Vector2(0.0f, 124.0f * 2), 1, 25, 20, 1, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightJump", RightImage, Vector2(0.0f, 124.0f * 3), 1, 25, 20, 2, Vector2(-85.0f, -50.0f), 1);
		mAnimator->CreateAnimation(L"RightFall", RightImage, Vector2(0.0f, 124.0f * 4), 1, 25, 20, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightFallRepeat", RightImage, Vector2(0.0f, 124.0f * 5), 1, 25, 20, 3, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightAttackA", RightImage, Vector2(0.0f, 124.0f * 6), 1, 25, 20, 5, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightAttackB", RightImage, Vector2(0.0f, 124.0f * 7), 1, 25, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightJumpAttack", RightImage, Vector2(0.0f, 124.0f * 8), 1, 25, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightSkillA", RightImage, Vector2(0.0f, (124.0f * 9)), 1, 25, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightSkillS", RightImage, Vector2(0.0f, (124.0f * 12)), 1, 25, 20, 9, Vector2(-85.0f, -50.0f), 0.05);


		mAnimator->GetStartEvent(L"LeftSkillA") = std::bind(&Skul::StartSkillA, this);
		mAnimator->GetCompleteEvent(L"LeftSkillA") = std::bind(&Skul::CompleteSkillA, this);
		mAnimator->GetEndEvent(L"LeftSkillA") = std::bind(&Skul::EndSkillA, this);

		mAnimator->GetStartEvent(L"RightSkillA") = std::bind(&Skul::StartSkillA, this);
		mAnimator->GetCompleteEvent(L"RightSkillA") = std::bind(&Skul::CompleteSkillA, this);
		mAnimator->GetEndEvent(L"RightSkillA") = std::bind(&Skul::EndSkillA, this);


		mAnimator->GetStartEvent(L"LeftSkillS") = std::bind(&Skul::StartSkillS, this);
		mAnimator->GetCompleteEvent(L"LeftSkillS") = std::bind(&Skul::CompleteSkillS, this);
		mAnimator->GetEndEvent(L"LeftSkillS") = std::bind(&Skul::EndSkillS, this);

		mAnimator->GetStartEvent(L"RightSkillS") = std::bind(&Skul::StartSkillS, this);
		mAnimator->GetCompleteEvent(L"RightSkillS") = std::bind(&Skul::CompleteSkillS, this);
		mAnimator->GetEndEvent(L"RightSkillS") = std::bind(&Skul::EndSkillS, this);

		Skul::Initialize();

	}
	void Basic::Update()
	{
		
		Skul::Update();
	}
	void Basic::Render(HDC hdc)
	{
		Skul::Render(hdc);

	}
	void Basic::Release()
	{
		Skul::Release();
	}
	void Basic::SkillA()
	{
		if (!(mbPlay))
		{
			mbPlay = true;

			if (mDirect == eDirection::Left)
				mAnimator->Play(L"LeftSkillA", false);
			else
				mAnimator->Play(L"RightSkillA", false);

			Transform* tr = Skul::GetComponent<Transform>();
			Scene* ActiveScene = SceneManager::GetActiveScene();

			mSkulHead = dynamic_cast<SkulHead*>(ActiveScene->GetGameObjects(eLayerType::Bullet)[0]);
			mSkulHead->SetState(eState::Active);
			mSkulHead->GetComponent<Transform>()->SetPos(tr->GetPos());

			if (mDirect == eDirection::Left)
				mSkulHead->SetDirect(eDirection::Left);

			else if (mDirect == eDirection::Right)
				mSkulHead->SetDirect(eDirection::Right);

		}
	}
	void Basic::SkillS()
	{
		if (!(mbPlay))
		{
			mbPlay = true;

			if (mDirect == eDirection::Left)
				mAnimator->Play(L"LeftSkillS", false);
			else
				mAnimator->Play(L"RightSkillS", false);
		}
	}
	void Basic::StartSkillA()
	{



	}

	void Basic::CompleteSkillA()
	{
		Skul* skul = SwitchSkul(eSkulType::Nohead);
		
		skul->SetSkulState(eSkulState::Idle);

		mbPlay = false;
	}

	void Basic::EndSkillA()
	{
	}
	void Basic::StartSkillS()
	{
	}
	void Basic::CompleteSkillS()
	{
		mState = eSkulState::Idle;
		
		if (mDirect == eDirection::Left)
			mAnimator->Play(L"LeftIdle", true);

		else
			mAnimator->Play(L"RightIdle", true);
		
		mbPlay = false;
	}
	void Basic::EndSkillS()
	{
	}
}
