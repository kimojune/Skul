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

		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimation(L"LeftIdle", LeftImage, Vector2(2356.0f, 0.0f), -1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftRun", LeftImage, Vector2(2356.0f, 124.0f), -1, 20, 20, 8, Vector2(-85.0f, -50.0f), 0.08);
		mAnimator->CreateAnimation(L"LeftDash", LeftImage, Vector2(2356.0f, 124.0f * 2), -1, 20, 20, 1, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftJump", LeftImage, Vector2(2356.0f, 124.0f * 3), -1, 20, 20, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftFall", LeftImage, Vector2(2356.0f, 124.0f * 4), -1, 20, 20, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftFallRepeat", LeftImage, Vector2(2356.0f, 124.0f * 5), -1, 20, 20, 3, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftAttackA", LeftImage, Vector2(2356.0f, 124.0f * 6), -1, 20, 20, 5, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftAttackB", LeftImage, Vector2(2356.0f, 124.0f * 7), -1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftAttackC", LeftImage, Vector2(2356.0f, 124.0f * 8), -1, 20, 20, 5, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftSkillA", LeftImage, Vector2(2356.0f, (124.0f * 9)), -1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.1);

		mAnimator->CreateAnimation(L"RightIdle", RightImage, Vector2(0.0f, 0.0f), 1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightRun", RightImage, Vector2(0.0f, 124.0f), 1, 20, 20, 8, Vector2(-85.0f, -50.0f), 0.08);
		mAnimator->CreateAnimation(L"RightDash", RightImage, Vector2(0.0f, 124.0f * 2), 1, 20, 20, 1, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightJump", RightImage, Vector2(0.0f, 124.0f * 3), 1, 20, 20, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightFall", RightImage, Vector2(0.0f, 124.0f * 4), 1, 20, 20, 2, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightFallRepeat", RightImage, Vector2(0.0f, 124.0f * 5), 1, 20, 20, 3, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightAttackA", RightImage, Vector2(0.0f, 124.0f * 6), 1, 20, 20, 5, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightAttackB", RightImage, Vector2(0.0f, 124.0f * 7), 1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightAttackC", RightImage, Vector2(0.0f, 124.0f * 8), 1, 20, 20, 5, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightSkillA", RightImage, Vector2(0.0f, (124.0f * 9)), 1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.1);


		//mAnimator->CreateAnimation(L"FowardRight", mImage, Vector2(0.0f, 113.0f), 16, 8, 15, Vector2::Zero, 0.1);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Idle", Vector2::Zero, 0.1f);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Aim\\Straight", Vector2::Zero, 0.1f);
		mAnimator->GetStartEvent(L"LeftDash") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"LeftDash") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"LeftDash") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"RightDash") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"RightDash") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"RightDash") = std::bind(&Skul::EndAttack, this);


		mAnimator->GetStartEvent(L"LeftJump") = std::bind(&Skul::StartJump, this);
		mAnimator->GetCompleteEvent(L"LeftJump") = std::bind(&Skul::CompleteJump, this);
		mAnimator->GetEndEvent(L"LeftJump") = std::bind(&Skul::EndJump, this);

		mAnimator->GetStartEvent(L"RightJump") = std::bind(&Skul::StartJump, this);
		mAnimator->GetCompleteEvent(L"RightJump") = std::bind(&Skul::CompleteJump, this);
		mAnimator->GetEndEvent(L"RightJump") = std::bind(&Skul::EndJump, this);

		mAnimator->GetStartEvent(L"LeftAttackA") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"LeftAttackA") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"LeftAttackA") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"RightAttackA") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"RightAttackA") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"RightAttackA") = std::bind(&Skul::EndAttack, this);






		mAnimator->GetStartEvent(L"LeftSkillA") = std::bind(&Skul::StartShoot, this);
		mAnimator->GetCompleteEvent(L"LeftSkillA") = std::bind(&Skul::CompleteShoot, this);
		mAnimator->GetEndEvent(L"LeftSkillA") = std::bind(&Skul::EndShoot, this);

		mAnimator->GetStartEvent(L"RightSkillA") = std::bind(&Skul::StartShoot, this);
		mAnimator->GetCompleteEvent(L"RightSkillA") = std::bind(&Skul::CompleteShoot, this);
		mAnimator->GetEndEvent(L"RightSkillA") = std::bind(&Skul::EndShoot, this);
		mAnimator->Play(L"RightIdle", true);

		Collider* collider = AddComponent<Collider>();
		collider->SetCenter(Vector2(-20.0f, -25.0f));
		collider->SetSize(Vector2(50.0f, 50.0f));

		mRigidbody = AddComponent<Rigidbody>();
		mRigidbody->SetMass(1.0f);


		mState = eSkulState::Idle;



		GameObject::Initialize();
	}


	void Skul::Update()
	{
		if (Input::GetKeyDown(eKeyCode::LEFT))
		{
			mDirect = eDirection::Left;
		}
		else if (Input::GetKeyDown(eKeyCode::RIGHT))
		{
			mDirect = eDirection::Right;
		}

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
				mAnimator->Play(L"LeftRun", true);
			}

			if (Input::GetKey(eKeyCode::RIGHT))
			{
				mState = eSkulState::Move;
				mDirect = eDirection::Right;
				mAnimator->Play(L"RightRun", true);
			}
		}

		if (Input::GetKeyDown(eKeyCode::Z))
		{
			mState = eSkulState::Dash;

			switch (mDirect)
			{
			case eDirection::Left:
				mAnimator->Play(L"LeftDash", true);
				break;

			case eDirection::Right:
				mAnimator->Play(L"RightDash", true);
				break;

			default:
				break;

			}

		}

		if (Input::GetKeyDown(eKeyCode::X))
		{
			mState = eSkulState::Attack;

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

		if (Input::GetKeyDown(eKeyCode::C))
		{
			mState = eSkulState::Jump;

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


		if (Input::GetKeyDown(eKeyCode::A))
		{
			mState = eSkulState::SkillA;

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

		/*if (Input::GetKeyDown(eKeyCode::LEFT)
			|| Input::GetKeyDown(eKeyCode::RIGHT))
		{
			if (Input::GetKeyDown(eKeyCode::LEFT))
			{
				mDirect = eDirection::Left;
				mAnimator->Play(L"LeftRun", true);
			}

			if (Input::GetKeyDown(eKeyCode::RIGHT))
			{
				mDirect = eDirection::Right;
				mAnimator->Play(L"RightRun", true);
			}
		}*/

		if (Input::GetKey(eKeyCode::LEFT)
			&& Input::GetKey(eKeyCode::RIGHT))
		{
			mState = eSkulState::Idle;

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

		if (Input::GetKeyDown(eKeyCode::A))
		{

			mState = eSkulState::SkillA;

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

		//이동 부
		if (mState == eSkulState::Move)
		{
			Transform* tr = GetComponent<Transform>();
			Vector2 pos = tr->GetPos();

			if (Input::GetKey(eKeyCode::LEFT))
			{
				//pos.x -= 200.0f * Time::DeltaTime();
				mRigidbody->AddForce(Vector2(-200.0f, 0.0f));
			}

			if (Input::GetKey(eKeyCode::RIGHT))
			{
				//pos.x += 200.0f * Time::DeltaTime();
				mRigidbody->AddForce(Vector2(200.0f, 0.0f));

			}
			tr->SetPos(pos);

		}
	}

	void Skul::Dash()
	{
		Vector2 velocity = mRigidbody->GetVelocity();
		velocity.x -= 500.0f;
		mRigidbody->SetVelocity(velocity);
	}
	void Skul::Jump()
	{

	}
	void Skul::Fall()
	{
	}
	void Skul::Fallrepeat()
	{
	}
	void Skul::Attack()
	{


	}


	void Skul::SkillA()
	{

	}
	void Skul::SkillB()
	{
	}

	void Skul::Jumpattack()
	{
	}

	////void Skul::death()
	//{
	//}


	// Event
	void Skul::StartDash()
	{

	}

	void Skul::CompleteDash()
	{
		mState = eSkulState::Idle;

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

	void Skul::EndDash()
	{
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
			mAnimator->Play(L"LeftIdle", true);
			break;
		case eDirection::Right:
			mAnimator->Play(L"RightIdle", true);
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
		mState = eSkulState::Idle;

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

	void Skul::EndAttack()
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
		mState = eSkulState::Idle;

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
	void Skul::EndJump()
	{
	}

}