#pragma once


#include "yaBasicSkul.h"
#include "yaResources.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaSceneManager.h"
#include "yaPlayeScene.h"
#include "yaSkulHead.h"
#include "yaObject.h"
#include "yaTime.h"
#include "yaSound.h"
#include "yaRigidbody.h"
#include "yaPlayerStats.h"


namespace ya
{


	Basic::Basic()
		:mbPlay(false)
		, mSkulHead(nullptr)
		, mTime(0.0f)
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

		Skul_SkullThrowing = Resources::Load<Sound>(L"Skul_SkullThrowing", L"..\\Resources\\Sound\\Skul\\Skul_SkullThrowing.wav");
		// 전에는 키가 "Skul_SkullThrowing"이라 던지는 소리가 대신 나왔다
		Skul_Reborn_Rise = Resources::Load<Sound>(L"Skul_Reborn_Rise", L"..\\Resources\\Sound\\Skul\\Skul_Reborn_Rise.wav");
	

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
		mAnimator->CreateAnimation(L"LeftChangeSkill", LeftImage, Vector2(2976.0f, (124.0f * 14)), -1, 25, 20, 7, Vector2(-85.0f, -50.0f), 0.05);
		// 사망: 시트 13번째 줄 22장 (무너져 뼈 더미가 된 뒤 하얗게 빛나며 사라짐), 0.07초씩 약 1.5초
		mAnimator->CreateAnimation(L"LeftDead", LeftImage, Vector2(2976.0f, (124.0f * 13)), -1, 25, 20, 22, Vector2(-85.0f, -50.0f), 0.07f);

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
		mAnimator->CreateAnimation(L"RightChangeSkill", RightImage, Vector2(0.0f, (124.0f * 14)), 1, 25, 20, 7, Vector2(-85.0f, -50.0f), 0.05);
		mAnimator->CreateAnimation(L"RightDead", RightImage, Vector2(0.0f, (124.0f * 13)), 1, 25, 20, 22, Vector2(-85.0f, -50.0f), 0.07f);


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
		
		mAnimator->GetStartEvent(L"LeftChangeSkill") = std::bind(&Skul::StartChangeSkill, this);
		mAnimator->GetCompleteEvent(L"LeftChangeSkill") = std::bind(&Skul::CompleteChangeSkill, this);
		mAnimator->GetEndEvent(L"LeftChangeSkill") = std::bind(&Skul::EndChangeSkill, this);

		mAnimator->GetStartEvent(L"RightChangeSkill") = std::bind(&Skul::StartChangeSkill, this);
		mAnimator->GetCompleteEvent(L"RightChangeSkill") = std::bind(&Skul::CompleteChangeSkill, this);
		mAnimator->GetEndEvent(L"RightChangeSkill") = std::bind(&Skul::EndChangeSkill, this);

		


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
	bool Basic::CanUseSkillA()
	{
		return PlayerStats::CanThrowHead(GetComponent<Rigidbody>()->GetGround());
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

			// 머리는 플레이 씬마다 하나씩 있다 (PlayeScene::Initialize)
			PlayeScene* playscene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
			mSkulHead = playscene->GetSkulHead();
			mSkulHead->Throw(tr->GetPos(), mDirect);
			// 머리가 날아가는 순간에 소리를 낸다. 전에는 던지는 애니메이션이 끝날 때(CompleteSkillA, 약 0.4초 뒤) 나서 늦게 들렸다
			Skul_SkullThrowing->Play(false);

			// 쿨다운 6초 시작. 공중이면 착지 전까지 다시 못 던진다
			PlayerStats::OnHeadThrown(GetComponent<Rigidbody>()->GetGround());
		}
	}
	void Basic::SkillS()
	{
		// S로 머리 위치에 순간이동한 직후(Nohead::SkillS)에만 들어온다. 누워 있다 일어나는 애니메이션
		if (!(mbPlay))
		{
			mbPlay = true;

			if (mDirect == eDirection::Left)
				mAnimator->Play(L"LeftSkillS", false);
			else
				mAnimator->Play(L"RightSkillS", false);
		}
	}
	void Basic::ChangeSkill()
	{
		if (!(mbPlay))
		{
			mbPlay = true;

			if (mDirect == eDirection::Left)
				mAnimator->Play(L"LeftChangeSkill", true);
			else
				mAnimator->Play(L"RightChangeSkill", true);
		}
		Transform* tr = GetComponent<Transform>();
		Vector2 skulpos = tr->GetPos();
		
		if (mDirect == eDirection::Left)
			skulpos.x -= 100 * Time::DeltaTime();
		else
			skulpos.x += 100 * Time::DeltaTime();

		mTime += Time::DeltaTime();
		
		tr->SetPos(skulpos);

		if (mTime > 2)
		{
			mState = eSkulState::Idle;
			if (mDirect == eDirection::Left)
				mAnimator->Play(L"LeftIdle", true);
			else
				mAnimator->Play(L"RightIdle", true);
			mTime = 0;
		}
	}
	void Basic::StartSkillA()
	{
		if (!(mbPlay))
		{
			mbPlay = true;

			if (mDirect == eDirection::Left)
				mAnimator->Play(L"LeftSkillA", false);
			else
				mAnimator->Play(L"RightSkillA", false);
		}

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
		Skul_Reborn_Rise->Play(false);
	}

	void Basic::CompleteSkillS()
	{
		mbPlay = false;
		mState = eSkulState::Idle;
		if (mDirect == eDirection::Left)
			mAnimator->Play(L"LeftIdle", true);
		else
			mAnimator->Play(L"RightIdle", true);

	}
	
	void Basic::EndSkillS()
	{
	}

	void Basic::Die()
	{
		// 던지기·일어나기 도중에 쓰러지면 완료 이벤트가 오지 않으므로 여기서 풀어 둔다 (다시 시작했을 때 스킬이 막히지 않게)
		mbPlay = false;
		Skul::Die();
	}
	void Basic::StartChangeSkill()
	{

	}
	void Basic::CompleteChangeSkill()
	{
	}
	void Basic::EndChangeSkill()
	{
		mbPlay = false;
	}
}
