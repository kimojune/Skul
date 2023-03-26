#include "yaSkul.h"
#include "yaTime.h"
#include "yaSceneManager.h"
#include "yaInput.h"
#include "yaResources.h"
#include "yaTransform.h"
#include "yaAnimator.h"
#include "yaCollider.h"
#include "yaSkulHead.h"
#include "yaScene.h"
#include "yaObject.h"
#include "yaRigidbody.h"


namespace ya
{
	Skul::Skul()
		: mState(eSkulState::Idle)
		, head(true)
	{
	}
	Skul::~Skul()
	{
	}

	void Skul::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.5f, 2.5f));

		mDirect = eDirection::Right;

		Image* LeftImage = Resources::Load<Image>(L"LeftSkul", L"..\\Resources\\SkulLeft.bmp");
		Image* RightImage = Resources::Load<Image>(L"RightSkul", L"..\\Resources\\SkulRight.bmp");
		Image* LeftHeadlessImage = Resources::Load<Image>(L"LeftHeadless", L"..\\Resources\\LeftHeadless.bmp");
		Image* RightHeadlessImage = Resources::Load<Image>(L"RightHeadless", L"..\\Resources\\RightHeadless.bmp");

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

		mAnimator->CreateAnimation(L"NoheadLeftIdle", LeftHeadlessImage, Vector2(1116.0f, 0.0f), -1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadLeftRun", LeftHeadlessImage, Vector2(1116.0f, 124.0f), -1, 10, 10, 8, Vector2(-85.0f, -50.0f), 0.08);
		mAnimator->CreateAnimation(L"NoheadLeftDash", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 2), -1, 10, 10, 1, Vector2(-85.0f, -50.0f), 1);
		mAnimator->CreateAnimation(L"NoheadLeftJump", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 3), -1, 10, 10, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadLeftFall", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 4), -1, 10, 10, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadLeftFallRepeat", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 5), -1, 10, 10, 3, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadLeftAttackA", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 6), -1, 10, 10, 5, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadLeftAttackB", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 7), -1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadLeftJumpAttack", LeftHeadlessImage, Vector2(1116.0f, 124.0f * 8), -1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);

		mAnimator->CreateAnimation(L"NoheadRightIdle", RightHeadlessImage, Vector2(0.0f, 0.0f), 1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadRightRun", RightHeadlessImage, Vector2(0.0f, 124.0f), 1, 10, 10, 8, Vector2(-85.0f, -50.0f), 0.08);
		mAnimator->CreateAnimation(L"NoheadRightDash", RightHeadlessImage, Vector2(0.0f, 124.0f * 2), 1, 10, 10, 1, Vector2(-85.0f, -50.0f), 1);
		mAnimator->CreateAnimation(L"NoheadRightJump", RightHeadlessImage, Vector2(0.0f, 124.0f * 3), 1, 10, 10, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadRightFall", RightHeadlessImage, Vector2(0.0f, 124.0f * 4), 1, 10, 20, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadRightFallRepeat", RightHeadlessImage, Vector2(0.0f, 124.0f * 5), 1, 10, 10, 3, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadRightAttackA", RightHeadlessImage, Vector2(0.0f, 124.0f * 6), 1, 10, 10, 5, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadRightAttackB", RightHeadlessImage, Vector2(0.0f, 124.0f * 7), 1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"NoheadRightJumpAttack", RightHeadlessImage, Vector2(0.0f, 124.0f * 8), 1, 10, 10, 4, Vector2(-85.0f, -50.0f), 0.1);


		//mAnimator->CreateAnimation(L"FowardRight", mImage, Vector2(0.0f, 113.0f), 16, 8, 15, Vector2::Zero, 0.1);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Idle", Vector2::Zero, 0.1f);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Aim\\Straight", Vector2::Zero, 0.1f);
		mAnimator->GetStartEvent(L"LeftDash") = std::bind(&Skul::StartDash, this);
		mAnimator->GetCompleteEvent(L"LeftDash") = std::bind(&Skul::CompleteDash, this);
		mAnimator->GetEndEvent(L"LeftDash") = std::bind(&Skul::EndDash, this);

		mAnimator->GetStartEvent(L"RightDash") = std::bind(&Skul::StartDash, this);
		mAnimator->GetCompleteEvent(L"RightDash") = std::bind(&Skul::CompleteDash, this);
		mAnimator->GetEndEvent(L"RightDash") = std::bind(&Skul::EndDash, this);

		mAnimator->GetStartEvent(L"NoheadLeftDash") = std::bind(&Skul::StartDash, this);
		mAnimator->GetCompleteEvent(L"NoheadLeftDash") = std::bind(&Skul::CompleteDash, this);
		mAnimator->GetEndEvent(L"NoheadLeftDash") = std::bind(&Skul::EndDash, this);

		mAnimator->GetStartEvent(L"NoheadRightDash") = std::bind(&Skul::StartDash, this);
		mAnimator->GetCompleteEvent(L"NoheadRightDash") = std::bind(&Skul::CompleteDash, this);
		mAnimator->GetEndEvent(L"NoheadRightDash") = std::bind(&Skul::EndDash, this);



		mAnimator->GetStartEvent(L"LeftJump") = std::bind(&Skul::StartJump, this);
		mAnimator->GetCompleteEvent(L"LeftJump") = std::bind(&Skul::CompleteJump, this);
		mAnimator->GetEndEvent(L"LeftJump") = std::bind(&Skul::EndJump, this);

		mAnimator->GetStartEvent(L"RightJump") = std::bind(&Skul::StartJump, this);
		mAnimator->GetCompleteEvent(L"RightJump") = std::bind(&Skul::CompleteJump, this);
		mAnimator->GetEndEvent(L"RightJump") = std::bind(&Skul::EndJump, this);

		mAnimator->GetStartEvent(L"NoheadLeftJump") = std::bind(&Skul::StartJump, this);
		mAnimator->GetCompleteEvent(L"NoheadLeftJump") = std::bind(&Skul::CompleteJump, this);
		mAnimator->GetEndEvent(L"NoheadLeftJump") = std::bind(&Skul::EndJump, this);

		mAnimator->GetStartEvent(L"NoheadRightJump") = std::bind(&Skul::StartJump, this);
		mAnimator->GetCompleteEvent(L"NoheadRightJump") = std::bind(&Skul::CompleteJump, this);
		mAnimator->GetEndEvent(L"NoheadRightJump") = std::bind(&Skul::EndJump, this);



		mAnimator->GetStartEvent(L"LeftFall") = std::bind(&Skul::StartFall, this);
		mAnimator->GetCompleteEvent(L"LeftFall") = std::bind(&Skul::CompleteFall, this);
		mAnimator->GetEndEvent(L"LeftFall") = std::bind(&Skul::EndFall, this);

		mAnimator->GetStartEvent(L"RightFall") = std::bind(&Skul::StartFall, this);
		mAnimator->GetCompleteEvent(L"RightFall") = std::bind(&Skul::CompleteFall, this);
		mAnimator->GetEndEvent(L"RightFall") = std::bind(&Skul::EndFall, this);

		mAnimator->GetStartEvent(L"NoheadLeftFall") = std::bind(&Skul::StartFall, this);
		mAnimator->GetCompleteEvent(L"NoheadLeftFall") = std::bind(&Skul::CompleteFall, this);
		mAnimator->GetEndEvent(L"NoheadLeftFall") = std::bind(&Skul::EndFall, this);

		mAnimator->GetStartEvent(L"NoheadRightFall") = std::bind(&Skul::StartFall, this);
		mAnimator->GetCompleteEvent(L"NoheadRightFall") = std::bind(&Skul::CompleteFall, this);
		mAnimator->GetEndEvent(L"NoheadRightFall") = std::bind(&Skul::EndFall, this);



		mAnimator->GetStartEvent(L"LeftAttackA") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"LeftAttackA") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"LeftAttackA") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"RightAttackA") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"RightAttackA") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"RightAttackA") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"NoheadLeftAttackA") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"NoheadLeftAttackA") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"NoheadLeftAttackA") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"NoheadRightAttackA") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"NoheadRightAttackA") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"NoheadRightAttackA") = std::bind(&Skul::EndAttack, this);


		mAnimator->GetStartEvent(L"LeftAttackB") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"LeftAttackB") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"LeftAttackB") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"RightAttackB") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"RightAttackB") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"RightAttackB") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"NoheadLeftAttackB") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"NoheadLeftAttackB") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"NoheadLeftAttackB") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"NoheadRightAttackB") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"NoheadRightAttackB") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"NoheadRightAttackB") = std::bind(&Skul::EndAttack, this);




		mAnimator->GetStartEvent(L"LeftJumpAttack") = std::bind(&Skul::StartJumpAttack, this);
		mAnimator->GetCompleteEvent(L"LeftJumpAttack") = std::bind(&Skul::CompleteJumpAttack, this);
		mAnimator->GetEndEvent(L"LeftJumpAttack") = std::bind(&Skul::EndJumpAttack, this);

		mAnimator->GetStartEvent(L"RightJumpAttack") = std::bind(&Skul::StartJumpAttack, this);
		mAnimator->GetCompleteEvent(L"RightJumpAttack") = std::bind(&Skul::CompleteJumpAttack, this);
		mAnimator->GetEndEvent(L"RightJumpAttack") = std::bind(&Skul::EndJumpAttack, this);

		mAnimator->GetStartEvent(L"NoheadLeftJumpAttack") = std::bind(&Skul::StartJumpAttack, this);
		mAnimator->GetCompleteEvent(L"NoheadLeftJumpAttack") = std::bind(&Skul::CompleteJumpAttack, this);
		mAnimator->GetEndEvent(L"NoheadLeftJumpAttack") = std::bind(&Skul::EndJumpAttack, this);

		mAnimator->GetStartEvent(L"NoheadRightJumpAttack") = std::bind(&Skul::StartJumpAttack, this);
		mAnimator->GetCompleteEvent(L"NoheadRightJumpAttack") = std::bind(&Skul::CompleteJumpAttack, this);
		mAnimator->GetEndEvent(L"NoheadRightJumpAttack") = std::bind(&Skul::EndJumpAttack, this);






		mAnimator->GetStartEvent(L"LeftSkillA") = std::bind(&Skul::StartShoot, this);
		mAnimator->GetCompleteEvent(L"LeftSkillA") = std::bind(&Skul::CompleteShoot, this);
		mAnimator->GetEndEvent(L"LeftSkillA") = std::bind(&Skul::EndShoot, this);

		mAnimator->GetStartEvent(L"RightSkillA") = std::bind(&Skul::StartShoot, this);
		mAnimator->GetCompleteEvent(L"RightSkillA") = std::bind(&Skul::CompleteShoot, this);
		mAnimator->GetEndEvent(L"RightSkillA") = std::bind(&Skul::EndShoot, this);


		Collider* collider = AddComponent<Collider>();
		collider->SetCenter(Vector2(-20.0f, -25.0f));
		collider->SetSize(Vector2(50.0f, 50.0f));

		mRigidbody = AddComponent<Rigidbody>();
		mRigidbody->SetMass(1.0f);
		mRigidbody->SetGround(false);

		mState = eSkulState::Fall;
		mAnimator->Play(L"RightFall", false);
		


		GameObject::Initialize();
	}


	void Skul::Update()
	{
		Transform* tr = GetComponent<Transform>();

		Vector2 pos = tr->GetPos();


		//if (Input::GetKeyDown(eKeyCode::LEFT))
		//{
		//	mDirect = eDirection::Left;
		//}
		//else if (Input::GetKeyDown(eKeyCode::RIGHT))
		//{
		//	mDirect = eDirection::Right;
		//}

		switch (mState)
		{
		case ya::Skul::eSkulState::Idle:
			Idle();
			break;
		case ya::Skul::eSkulState::Move:
			Move();
			break;
		case ya::Skul::eSkulState::Dash:
			Dash();
			break;
		case ya::Skul::eSkulState::Jump:
			Jump();
			break;
		case ya::Skul::eSkulState::Fall:
			Fall();
		case ya::Skul::eSkulState::Attack:
			Attack();
			break;
		case ya::Skul::eSkulState::JumpAttack:
			Jumpattack();
			break;
		case ya::Skul::eSkulState::SkillA:
			SkillA();
			break;
		default:
			break;
		}
		GameObject::Update();
	}

	void Skul::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void Skul::Release()
	{
		GameObject::Release();

	}

	void Skul::OnCollisionEnter(Collider* other)
	{
	}

	void Skul::OnCollisionStay(Collider* other)
	{
	}

	void Skul::OnCollisionExit(Collider* other)
	{
	}



	/*key


	Z 대쉬
	X 공격
	C 점프
	A 스킬1
	S 스킬1
	D 정수
	F 상호작용
	SPACE 교대



	*/


	void Skul::Idle()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();

		if (
			!(Input::GetKey(eKeyCode::LEFT) && Input::GetKey(eKeyCode::RIGHT))
			)
		{

			if (Input::GetKey(eKeyCode::LEFT))
			{
				mState = eSkulState::Move;
				mDirect = eDirection::Left;

				if (head)
				{
					mAnimator->Play(L"LeftRun", true);
				}
				else
				{
					mAnimator->Play(L"NoheadLeftRun", true);
				}
			}

			if (Input::GetKey(eKeyCode::RIGHT))
			{
				mState = eSkulState::Move;
				mDirect = eDirection::Right;

				if (head)
				{
					mAnimator->Play(L"RightRun", true);
				}
				else
				{
					mAnimator->Play(L"NoheadRightRun", true);
				}
			}
		}

		if (!(mRigidbody->GetGround()))
		{
			mState = eSkulState::Fall;


			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftFall", false);
					break;
				case eDirection::Right:
					mAnimator->Play(L"RightFall", false);
					break;
				default:
					break;
				}
			}
			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"NoheadLeftFall", false);
					break;
				case eDirection::Right:
					mAnimator->Play(L"NoheadRightFall", false);
					break;
				default:
					break;
				}
			}
		}




		if (Input::GetKeyDown(eKeyCode::Z))
		{
			mState = eSkulState::Dash;


			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"LeftDash", false);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"RightDash", false);
					break;
				}
				}
			}

			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"NoheadLeftDash", true);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"NoheadRightDash", true);
					break;
				}
				}
			}
		}



		if (Input::GetKeyDown(eKeyCode::X))
		{
			mState = eSkulState::Attack;
			SecondAttack = true;

			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftAttackA", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"RightAttackA", false);
					break;

				default:
					break;
				}
			}
			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"NoheadLeftAttackA", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"NoheadRightAttackA", false);
					break;

				default:
					break;
				}
			}
		}



		if (Input::GetKeyDown(eKeyCode::C))
		{
			mState = eSkulState::Jump;

			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftJump", true);
					break;

				case eDirection::Right:
					mAnimator->Play(L"RightJump", true);
					break;

				default:
					break;
				}
			}

			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"NoheadLeftJump", true);
					break;

				case eDirection::Right:
					mAnimator->Play(L"NoheadRightJump", true);
					break;

				default:
					break;
				}
			}
		}

		if (head)
		{
			if (Input::GetKeyDown(eKeyCode::A))
			{
				mState = eSkulState::SkillA;
				head = false;
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftSkillA", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"RightSkillA", false);
					break;

				default:
					break;

				}
			}
		}

		//if (Input::GetKeyDown(eKeyCode::S))
		//{
		//	switch (mDirect)
		//	{
		//	case eDirection::Left:
		//		mAnimator->Play(L"LeftSkillB", false);
		//		break;

		//	case eDirection::Right:
		//		mAnimator->Play(L"RightSkillB", false);
		//		break;

		//	default:
		//		break;

		//	}

		//}
	}

	void Skul::Move()
	{

		if (Input::GetKeyNone(eKeyCode::LEFT)
			&& Input::GetKeyNone(eKeyCode::RIGHT))
		{
			mState = eSkulState::Idle;

			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"LeftIdle", true);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"RightIdle", true);
					break;
				}
				}
			}

			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"NoheadLeftIdle", true);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"NoheadRightIdle", true);
					break;
				}
				}
			}

		}



		if (Input::GetKey(eKeyCode::LEFT)
			&& Input::GetKey(eKeyCode::RIGHT))
		{
			mState = eSkulState::Idle;

			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"LeftIdle", true);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"RightIdle", true);
					break;
				}
				}
			}

			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"NoheadLeftIdle", true);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"NoheadRightIdle", true);
					break;
				}
				}
			}
		}


		if (Input::GetKeyDown(eKeyCode::Z))
		{
			mState = eSkulState::Dash;


			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"LeftDash", false);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"RightDash", false);
					break;
				}
				}
			}

			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"NoheadLeftDash", true);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"NoheadRightDash", true);
					break;
				}
				}
			}
		}



		if (Input::GetKeyDown(eKeyCode::X))
		{
			mState = eSkulState::Attack;
			SecondAttack = true;

			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftAttackA", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"RightAttackA", false);
					break;

				default:
					break;
				}
			}
			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"NoheadLeftAttackA", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"NoheadRightAttackA", false);
					break;

				default:
					break;
				}
			}
		}



		if (Input::GetKeyDown(eKeyCode::C))
		{
			mState = eSkulState::Jump;

			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftJump", true);
					break;

				case eDirection::Right:
					mAnimator->Play(L"RightJump", true);
					break;

				default:
					break;
				}
			}

			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"NoheadLeftJump", true);
					break;

				case eDirection::Right:
					mAnimator->Play(L"NoheadRightJump", true);
					break;

				default:
					break;
				}
			}
		}


		if (head)
		{
			if (Input::GetKeyDown(eKeyCode::A))
			{
				mState = eSkulState::SkillA;
				head = false;
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftSkillA", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"RightSkillA", false);
					break;

				default:
					break;

				}
			}
		}


		//이동 부
		if (mState == eSkulState::Move)
		{
			Transform* tr = GetComponent<Transform>();
			Vector2 pos = tr->GetPos();

			if (Input::GetKey(eKeyCode::LEFT))
			{
				mDirect = eDirection::Left;
				pos.x -= 200.0f * Time::DeltaTime();
				//mRigidbody->AddForce(Vector2(-200.0f, 0.0f));
			}

			if (Input::GetKey(eKeyCode::RIGHT))
			{
				mDirect = eDirection::Right;
				pos.x += 200.0f * Time::DeltaTime();
				//mRigidbody->AddForce(Vector2(200.0f, 0.0f));

			}
			tr->SetPos(pos);

		}
	}

	void Skul::Dash()
	{
		Vector2 velocity = mRigidbody->GetVelocity();

		mRigidbody->SetGround(true);

		if (mDirect == eDirection::Left && velocity.x == 0)
		{
			mState = eSkulState::Idle;
			
			if (head == false)
			{
				mAnimator->Play(L"NoheadLeftIdle", true);
			}

			else
			{
				mAnimator->Play(L"LeftIdle", true);
			}

		}

		else if (mDirect == eDirection::Right && velocity.x == 0)
		{
			mState = eSkulState::Idle;

			if (head == false)
			{
				mAnimator->Play(L"NoheadRightIdle", true);

			}
			else
			{
				mAnimator->Play(L"RightIdle", true);
			}
		}
	}
	void Skul::Jump()
	{
		Vector2 mVelocity = mRigidbody->GetVelocity();

		if (mVelocity.y >= 0)

		{
			mState = eSkulState::Fall;

			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftFall", false);
					break;
				case eDirection::Right:
					mAnimator->Play(L"RightFall", false);
					break;
				default:
					break;
				}
			}
			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"NoheadLeftFall", false);
					break;
				case eDirection::Right:
					mAnimator->Play(L"NoheadRightFall", false);
					break;
				default:
					break;
				}
			}
		}
		if (Input::GetKeyDown(eKeyCode::Z))
		{
			mState = eSkulState::Dash;


			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"LeftDash", false);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"RightDash", false);
					break;
				}
				}
			}

			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"NoheadLeftDash", true);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"NoheadRightDash", true);
					break;
				}
				}
			}

		}


		if (Input::GetKeyDown(eKeyCode::X))
		{
			mState = eSkulState::JumpAttack;

			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftJumpAttack", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"RightJumpAttack", false);
					break;

				default:
					break;
				}
			}
			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"NoheadLeftJumpAttack", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"NoheadRightJumpAttack", false);
					break;

				default:
					break;
				}
			}
		}

		//이동 부

		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();

		if (Input::GetKey(eKeyCode::LEFT))
		{
			pos.x -= 200.0f * Time::DeltaTime();
			//mRigidbody->AddForce(Vector2(-200.0f, 0.0f));
		}

		if (Input::GetKey(eKeyCode::RIGHT))
		{
			pos.x += 200.0f * Time::DeltaTime();
			//mRigidbody->AddForce(Vector2(200.0f, 0.0f));

		}
		tr->SetPos(pos);

	}
	void Skul::Fall()
	{
		Rigidbody* rd = GetComponent<Rigidbody>();

		if (rd->GetGround())
		{
			mState = eSkulState::Idle;

			if (head)
			{
			switch (mDirect)
			{
			case eDirection::Left:
				mAnimator->Play(L"LeftIdle", true);
				break;
			case eDirection::Right:
				mAnimator->Play(L"RightIdle", true);
				break;
			default:
				break;
			}
			}
			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"NoheadLeftIdle", true);
					break;
				case eDirection::Right:
					mAnimator->Play(L"NoheadRightIdle", true);
					break;
				default:
					break;
				}
			}
		}


		if (Input::GetKeyDown(eKeyCode::Z))
		{
			mState = eSkulState::Dash;


			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"LeftDash", false);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"RightDash", false);
					break;
				}
				}
			}

			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
				{
					mAnimator->Play(L"NoheadLeftDash", true);
					break;
				}
				case eDirection::Right:
				{
					mAnimator->Play(L"NoheadRightDash", true);
					break;
				}
				}
			}
		}



		if (Input::GetKeyDown(eKeyCode::X))
		{
			mState = eSkulState::JumpAttack;

			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftJumpAttack", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"RightJumpAttack", false);
					break;

				default:
					break;
				}
			}
			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"NoheadLeftJumpAttack", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"NoheadRightJumpAttack", false);
					break;

				default:
					break;
				}
			}
		}

		if (head)
		{
			if (Input::GetKeyDown(eKeyCode::A))
			{
				head = false;
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftSkillA", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"RightSkillA", false);
					break;

				default:
					break;

				}
			}
		}


		//이동 부
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();

		if (Input::GetKey(eKeyCode::LEFT))
		{
			pos.x -= 100.0f * Time::DeltaTime();
			/*		mRigidbody->AddForce(Vector2(-200.0f, 0.0f));*/
		}

		if (Input::GetKey(eKeyCode::RIGHT))
		{
			pos.x += 100.0f * Time::DeltaTime();
			/*		mRigidbody->AddForce(Vector2(200.0f, 0.0f));*/

		}
		tr->SetPos(pos);

	}

	void Skul::Attack()
	{
		if (AttackCount<2&&Input::GetKeyDown(eKeyCode::X))
		{
			AttackCount++;
		}

		Animation* mAnimation = mAnimator->GetAnimaiton();

		if (mAnimation->IsComplete())
		{
			if (AttackCount > 0)
			{
				AttackCount--;

				if (SecondAttack)
				{
					SecondAttack = false;

					if (head)
					{
						switch (mDirect)
						{

						case eDirection::Left:
							mAnimator->Play(L"LeftAttackB", false);
							break;

						case eDirection::Right:
							mAnimator->Play(L"RightAttackB", false);
							break;

						default:
							break;
						}
					}
					else
					{
						if (head)
						{
							switch (mDirect)
							{

							case eDirection::Left:
								mAnimator->Play(L"NoheadLeftAttackB", false);
								break;

							case eDirection::Right:
								mAnimator->Play(L"NoheadRightAttackB", false);
								break;

							default:
								break;
							}
						}
					}
				}
				else
				{
					SecondAttack = true;
					AttackCount--;

					if (head)
					{
						switch (mDirect)
						{

						case eDirection::Left:
							mAnimator->Play(L"LeftAttackA", false);
							break;

						case eDirection::Right:
							mAnimator->Play(L"RightAttackA", false);
							break;

						default:
							break;
						}
					}

					else
					{
						switch (mDirect)
						{

						case eDirection::Left:
							mAnimator->Play(L"NoheadLeftAttackA", false);
							break;

						case eDirection::Right:
							mAnimator->Play(L"NoheadRightAttackA", false);
							break;

						default:
							break;
						}
					}
				}
			}

			else
			{
				mState = eSkulState::Idle;
			
				if (head)
				{

				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftIdle", true);
					break;
				case eDirection::Right:
					mAnimator->Play(L"RightIdle", true);
					break;
				default:
					break;
				}
				}
				else
				{
					switch (mDirect)
					{
					case eDirection::Left:
						mAnimator->Play(L"NoheadLeftIdle", true);
						break;
					case eDirection::Right:
						mAnimator->Play(L"NoheadRightIdle", true);
						break;
					default:
						break;
					}
				}

			}

		}


	}
	void Skul::SkillA()
	{

	}
	void Skul::SkillB()
	{
	}

	void Skul::Jumpattack()
	{
		Rigidbody* rd = GetComponent<Rigidbody>();

		if (rd->GetGround())
		{
			mState = eSkulState::Idle;

			if (head)
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftIdle", true);
					break;
				case eDirection::Right:
					mAnimator->Play(L"RightIdle", true);
					break;
				default:
					break;
				}
			}
			else
			{
				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"NoheadLeftIdle", true);
					break;
				case eDirection::Right:
					mAnimator->Play(L"NoheadRightIdle", true);
					break;
				default:
					break;
				}
			}
		}

	}

	////void Skul::death()
	//{
	//}


	// Event
	void Skul::StartDash()
	{
		Vector2 velocity = mRigidbody->GetVelocity();
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();

		if (mDirect == eDirection::Left)
		{
			velocity.x = -7000;
			velocity.y = 0;
		}

		else if (mDirect == eDirection::Right)
		{
			velocity.x = 7000;
			velocity.y = 0;
		}
		mRigidbody->Setfriction(30000.0f);
		mRigidbody->SetVelocity(velocity);

	}

	void Skul::CompleteDash()
	{
	}

	void Skul::EndDash()
	{
		mRigidbody->Setfriction(100.0f);
		mRigidbody->SetGround(false);
	}



	void Skul::StartShoot()
	{
		Transform* tr = GetComponent<Transform>();
		Scene* curScene = SceneManager::GetActiveScene();
		SkulHead* head = new SkulHead();
		head->GetComponent<Transform>()->SetPos(tr->GetPos());

		if (mDirect == eDirection::Left)
			head->SetDirect(eDirection::Left);

		else if (mDirect == eDirection::Right)
			head->SetDirect(eDirection::Right);

		curScene->AddGameObeject(head, eLayerType::Bullet);

	}

	void Skul::CompleteShoot()
	{
		mState = eSkulState::Idle;

		switch (mDirect)
		{
		case eDirection::Left:
			mAnimator->Play(L"NoheadLeftIdle", true);
			break;
		case eDirection::Right:
			mAnimator->Play(L"NoheadRightIdle", true);
			break;
		default:
			break;
		}
	}

	void Skul::EndShoot()
	{
	}


	void Skul::StartAttack()
	{

	}
	
	void Skul::CompleteAttack()
	{
	}

	void Skul::EndAttack()
	{
	}

	void Skul::StartJumpAttack()
	{
	}

	void Skul::CompleteJumpAttack()
	{
		mState = eSkulState::Fall;
	   

		if (head)
		{
			switch (mDirect)
			{
			case eDirection::Left:
				mAnimator->Play(L"LeftFall", false);
				break;
			case eDirection::Right:
				mAnimator->Play(L"RightFall", false);
				break;
			default:
				break;
			}
		}
		else
		{
			switch (mDirect)
			{
			case eDirection::Left:
				mAnimator->Play(L"NoheadLeftFall", false);
				break;
			case eDirection::Right:
				mAnimator->Play(L"NoheadRightFall", false);
				break;
			default:
				break;
			}
		}
	}

	void Skul::EndJumpAttack()
	{
	}


	void Skul::StartJump()
	{
		Vector2 velocity = mRigidbody->GetVelocity();
		velocity.y -= 500.0f;

		mRigidbody->SetVelocity(velocity);
		mRigidbody->SetGround(false);
	}

	void Skul::CompleteJump()
	{

	}
	void Skul::EndJump()
	{
	}

	void Skul::StartFall()
	{

	}

	void Skul::CompleteFall()
	{
		if (head)
		{
			switch (mDirect)
			{
			case eDirection::Left:
				mAnimator->Play(L"LeftFallRepeat", true);
				break;
			case eDirection::Right:
				mAnimator->Play(L"RightFallRepeat", true);
				break;
			default:
				break;
			}
		}
		else
		{
			switch (mDirect)
			{
			case eDirection::Left:
				mAnimator->Play(L"NoheadLeftFallRepeat", true);
				break;
			case eDirection::Right:
				mAnimator->Play(L"NoheadRightFallRepeat", true);
				break;
			default:
				break;
			}
		}

	}

	void Skul::EndFall()
	{
	}

}