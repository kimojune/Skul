#pragma once


#include "yaNoheadSkul.h"
#include "yaResources.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaBasicSkul.h"
#include "yaSkulHead.h"
#include "yaCamera.h"
#include "yaPlayeScene.h"
#include "yaSceneManager.h"
#include "yaRigidbody.h"
#include "yaPlayerStats.h"
#include "yaSound.h"

namespace ya
{
	void Nohead::Initialize()
	{
		SetName(L"NoheadSkul");

		Image* LeftHeadlessImage = Resources::Load<Image>(L"LeftHeadless", L"..\\Resources\\LeftHeadless.bmp");
		Image* RightHeadlessImage = Resources::Load<Image>(L"RightHeadless", L"..\\Resources\\RightHeadless.bmp");
		Skul_SkullBack = Resources::Load<Sound>(L"Skul_SkullBack", L"..\\Resources\\Sound\\Skul\\Skul_SkullBack.wav");

		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimation(L"LeftIdle", LeftHeadlessImage, Vector2(1116.0f, 0.0f), -1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftRun", LeftHeadlessImage, Vector2(1116.0f, 124.0f), -1, 10, 10, 8, Vector2(-85.0f, -50.0f), 0.08);
		mAnimator->CreateAnimation(L"LeftDash", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 2), -1, 10, 10, 1, Vector2(-85.0f, -50.0f), 1);
		mAnimator->CreateAnimation(L"LeftJump", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 3), -1, 10, 10, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftFall", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 4), -1, 10, 10, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftFallRepeat", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 5), -1, 10, 10, 3, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftAttackA", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 6), -1, 10, 10, 5, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftAttackB", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 7), -1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftJumpAttack", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 8), -1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);

		mAnimator->CreateAnimation(L"RightIdle", RightHeadlessImage, Vector2(0.0f, 0.0f), 1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightRun", RightHeadlessImage, Vector2(0.0f, 124.0f), 1, 10, 10, 8, Vector2(-85.0f, -50.0f), 0.08);
		mAnimator->CreateAnimation(L"RightDash", RightHeadlessImage, Vector2(0.0f, 124.0f * 2), 1, 10, 10, 1, Vector2(-85.0f, -50.0f), 1);
		mAnimator->CreateAnimation(L"RightJump", RightHeadlessImage, Vector2(0.0f, 124.0f * 3), 1, 10, 10, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightFall", RightHeadlessImage, Vector2(0.0f, 124.0f * 4), 1, 10, 20, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightFallRepeat", RightHeadlessImage, Vector2(0.0f, 124.0f * 5), 1, 10, 10, 3, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightAttackA", RightHeadlessImage, Vector2(0.0f, 124.0f * 6), 1, 10, 10, 5, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightAttackB", RightHeadlessImage, Vector2(0.0f, 124.0f * 7), 1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightJumpAttack", RightHeadlessImage, Vector2(0.0f, 124.0f * 8), 1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);





		Skul::Initialize();
	}
	void Nohead::Update()
	{
		Skul::Update();
	}
	void Nohead::Render(HDC hdc)
	{
		Skul::Render(hdc);
	}
	void Nohead::Release()
	{
		Skul::Release();
	}
	void Nohead::SkillA()
	{
		mState = eSkulState::Idle;
	}
	bool Nohead::CanUseSkillS()
	{
		// 원작: 머리가 떨어져 있을 때만, 쿨다운 3초
		return PlayerStats::CanTeleport();
	}

	void Nohead::SkillS()
	{
		// 머리 위치로 순간이동하고 머리를 회수한다.
		// 원작에서 순간이동이 머리도 회수하는지는 불확실하다. 이 클론은 회수로 보고 A 쿨다운도 초기화한다.
		Basic* basic = dynamic_cast<Basic*>(mSkuls[(UINT)eSkulType::Basic]);
		PlayeScene* playscene = dynamic_cast<PlayeScene*> (SceneManager::GetActiveScene());
		SkulHead* head = playscene->GetSkulHead();

		PlayerStats::OnTeleported();
		PlayerStats::OnHeadRetrieved(true);

		basic->GetComponent<Transform>()->SetPos(head->GetTeleportPos());

		// 착지 상태는 복사하지 않는다. 전에는 Nohead의 착지 상태를 복사해서 공중으로 이동하면 몸이 떠 있었다.
		// 땅 위면 다음 충돌 검사에서 바로 착지한다
		Rigidbody* basicRigidbody = basic->GetComponent<Rigidbody>();
		basicRigidbody->SetGround(false);
		basicRigidbody->SetVelocity(Vector2::Zero);

		bool landedHead = head->GetHeadState() == SkulHead::eHeadState::Landed;
		head->Hide();

		playscene->SetSkul(eSkulType::Basic);
		basic->SetDirect(mDirect);

		// 바닥에 놓인 머리면 누워 있다 일어나는 애니메이션(SkillS), 공중이면 바로 떨어진다
		if (landedHead)
		{
			basic->SetSkulState(eSkulState::SkillS);
		}
		else
		{
			basic->SetSkulState(eSkulState::Fall);
			basic->GetComponent<Animator>()->Play(mDirect == eDirection::Left ? L"LeftFall" : L"RightFall", false);
		}

		mState = eSkulState::Idle;
		SetState(eState::Pause);
	}

	void Nohead::Die()
	{
		Skul* basic = TakeHeadBack(false, false);
		basic->Die();
	}

	bool Nohead::CanTakeHeadBack()
	{
		if (GetState() != eState::Active)
			return false;

		return mState != eSkulState::Dash
			&& mState != eSkulState::SkillS
			&& mState != eSkulState::Fall_Dead;
	}

	Skul* Nohead::TakeHeadBack(bool resetCooldown, bool playSound)
	{
		PlayeScene* playscene = dynamic_cast<PlayeScene*> (SceneManager::GetActiveScene());
		if (playscene != nullptr && playscene->GetSkulHead() != nullptr)
			playscene->GetSkulHead()->Hide();

		PlayerStats::OnHeadRetrieved(resetCooldown);
		if (playSound)
			Skul_SkullBack->Play(false);

		return SwitchSkul(eSkulType::Basic);
	}
}