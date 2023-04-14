#include "yaNoheadSkul.h"
#include "yaResources.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaBasicSkul.h"
#include "yaSkulHead.h"



namespace ya
{
	void Nohead::Initialize()
	{
		SetName(L"NoheadSkul");
		Image* LeftHeadlessImage = Resources::Load<Image>(L"LeftHeadless", L"..\\Resources\\LeftHeadless.bmp");
		Image* RightHeadlessImage = Resources::Load<Image>(L"RightHeadless", L"..\\Resources\\RightHeadless.bmp");

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
	void Nohead::SkillS()
	{
		Transform* tr = GetComponent<Transform>();

		Basic* basic = dynamic_cast<Basic*>(mSkuls[(UINT)eSkulType::Basic]);
		basic->SetState(eState::Active);

		SkulHead* head = basic->GetSkulHead();

		Transform* headPos = head->GetComponent<Transform>();
		

		basic->GetComponent<Transform>()->SetPos(headPos->GetPos());
		
		switch (mDirect)
		{
		case eDirection::Left:
			basic->GetComponent<Animator>()->Play(L"LeftSkillS",false);
			break;
		case eDirection::Right:
			basic->GetComponent<Animator>()->Play(L"RightSkillS", false);

			break;
		}

		mState = eSkulState::Idle;
		GameObject::SetState(eState::Pause);


	}
}