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
#include "yaPlayeScene.h"


namespace ya
{


	Skul::Skul()
		: mState(eSkulState::Idle)
		, mHead(true)
		, AttackCount(0)
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


		mAnimator = GetComponent<Animator>();


		mAnimator->GetStartEvent(L"LeftDash") = std::bind(&Skul::StartDash, this);
		mAnimator->GetCompleteEvent(L"LeftDash") = std::bind(&Skul::CompleteDash, this);
		mAnimator->GetEndEvent(L"LeftDash") = std::bind(&Skul::EndDash, this);

		mAnimator->GetStartEvent(L"RightDash") = std::bind(&Skul::StartDash, this);
		mAnimator->GetCompleteEvent(L"RightDash") = std::bind(&Skul::CompleteDash, this);
		mAnimator->GetEndEvent(L"RightDash") = std::bind(&Skul::EndDash, this);


		mAnimator->GetStartEvent(L"LeftJump") = std::bind(&Skul::StartJump, this);
		mAnimator->GetCompleteEvent(L"LeftJump") = std::bind(&Skul::CompleteJump, this);
		mAnimator->GetEndEvent(L"LeftJump") = std::bind(&Skul::EndJump, this);

		mAnimator->GetStartEvent(L"RightJump") = std::bind(&Skul::StartJump, this);
		mAnimator->GetCompleteEvent(L"RightJump") = std::bind(&Skul::CompleteJump, this);
		mAnimator->GetEndEvent(L"RightJump") = std::bind(&Skul::EndJump, this);


		mAnimator->GetStartEvent(L"LeftFall") = std::bind(&Skul::StartFall, this);
		mAnimator->GetCompleteEvent(L"LeftFall") = std::bind(&Skul::CompleteFall, this);
		mAnimator->GetEndEvent(L"LeftFall") = std::bind(&Skul::EndFall, this);

		mAnimator->GetStartEvent(L"RightFall") = std::bind(&Skul::StartFall, this);
		mAnimator->GetCompleteEvent(L"RightFall") = std::bind(&Skul::CompleteFall, this);
		mAnimator->GetEndEvent(L"RightFall") = std::bind(&Skul::EndFall, this);


		mAnimator->GetStartEvent(L"LeftAttackA") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"LeftAttackA") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"LeftAttackA") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"RightAttackA") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"RightAttackA") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"RightAttackA") = std::bind(&Skul::EndAttack, this);


		mAnimator->GetStartEvent(L"LeftAttackB") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"LeftAttackB") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"LeftAttackB") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"RightAttackB") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"RightAttackB") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"RightAttackB") = std::bind(&Skul::EndAttack, this);


		mAnimator->GetStartEvent(L"LeftJumpAttack") = std::bind(&Skul::StartJumpAttack, this);
		mAnimator->GetCompleteEvent(L"LeftJumpAttack") = std::bind(&Skul::CompleteJumpAttack, this);
		mAnimator->GetEndEvent(L"LeftJumpAttack") = std::bind(&Skul::EndJumpAttack, this);

		mAnimator->GetStartEvent(L"RightJumpAttack") = std::bind(&Skul::StartJumpAttack, this);
		mAnimator->GetCompleteEvent(L"RightJumpAttack") = std::bind(&Skul::CompleteJumpAttack, this);
		mAnimator->GetEndEvent(L"RightJumpAttack") = std::bind(&Skul::EndJumpAttack, this);


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
		case ya::Skul::eSkulState::SkillS:
			SkillS();
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

	void Skul::SwitchSkul(eSkulType type)
	{
		
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

			if (!(mRigidbody->GetGround()))
			{
				mState = eSkulState::Fall;


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




			if (Input::GetKeyDown(eKeyCode::Z))
			{
				mState = eSkulState::Dash;


				if (mHead)
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

			}



			if (Input::GetKeyDown(eKeyCode::X))
			{
				mState = eSkulState::Attack;
				SecondAttack = true;

				if (mHead)
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

			}



			if (Input::GetKeyDown(eKeyCode::C))
			{
				mState = eSkulState::Jump;

				if (mHead)
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


			}


			if (Input::GetKeyDown(eKeyCode::A))
			{
				mState = eSkulState::SkillA;
			}



			if (Input::GetKeyDown(eKeyCode::S))
			{
				mState = eSkulState::SkillS;

			}
		}
	}
	void Skul::Move()
	{

		if (Input::GetKeyNone(eKeyCode::LEFT)
			&& Input::GetKeyNone(eKeyCode::RIGHT))
		{
			mState = eSkulState::Idle;

			if (mHead)
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

		}



		if (Input::GetKey(eKeyCode::LEFT)
			&& Input::GetKey(eKeyCode::RIGHT))
		{
			mState = eSkulState::Idle;

			if (mHead)
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

		}


		if (Input::GetKeyDown(eKeyCode::Z))
		{
			mState = eSkulState::Dash;


			if (mHead)
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

		}



		if (Input::GetKeyDown(eKeyCode::X))
		{
			mState = eSkulState::Attack;
			SecondAttack = true;

			if (mHead)
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
		}


		if (Input::GetKeyDown(eKeyCode::S))
		{
			mState = eSkulState::SkillS;
		}

		//이동 부
		if (mState == eSkulState::Move)
		{
			Transform* tr = GetComponent<Transform>();
			Vector2 pos = tr->GetPos();
			Animation* mAnimation = mAnimator->GetAnimaiton();
			std::wstring mAnimationName = mAnimation->GetAnimationName();

			if (Input::GetKey(eKeyCode::LEFT))
			{
				mDirect = eDirection::Left;

				if (pos.x >= 0)
					pos.x -= 200.0f * Time::DeltaTime();

				if (!(mAnimationName == L"LeftRun"))
				{
					mAnimator->Play(L"LeftRun", true);
				}
				//mRigidbody->AddForce(Vector2(-200.0f, 0.0f));
			}

			if (Input::GetKey(eKeyCode::RIGHT))
			{
				mDirect = eDirection::Right;
				pos.x += 200.0f * Time::DeltaTime();

				//if (pos.x >= 0)
				//	pos.x -= 200.0f * Time::DeltaTime();
				//mRigidbody->AddForce(Vector2(200.0f, 0.0f));

				if (!(mAnimationName == L"RightRun"))
				{

					mAnimator->Play(L"RightRun", true);

				}
			}
			tr->SetPos(pos);

		}


	}

	void Skul::Dash()
	{
		Vector2 velocity = mRigidbody->GetVelocity();

		velocity.y = 0;

		mRigidbody->SetVelocity(velocity);

		if (mDirect == eDirection::Left && velocity.x == 0)
		{
			mState = eSkulState::Idle;

			mAnimator->Play(L"LeftIdle", true);

		}

		else if (mDirect == eDirection::Right && velocity.x == 0)
		{
			mState = eSkulState::Idle;

			mAnimator->Play(L"RightIdle", true);

		}
	}
	void Skul::Jump()
	{
		Vector2 mVelocity = mRigidbody->GetVelocity();

		if (mVelocity.y >= 0)

		{
			mState = eSkulState::Fall;


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
		if (Input::GetKeyDown(eKeyCode::Z))
		{
			mState = eSkulState::Dash;


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


		if (Input::GetKeyDown(eKeyCode::X))
		{
			mState = eSkulState::JumpAttack;


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


		if (Input::GetKeyDown(eKeyCode::Z))
		{
			mState = eSkulState::Dash;



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



		if (Input::GetKeyDown(eKeyCode::X))
		{
			mState = eSkulState::JumpAttack;


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


		if (Input::GetKeyDown(eKeyCode::A))
		{

		}

		if (Input::GetKeyDown(eKeyCode::S))
		{

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
		if (AttackCount < 1 && Input::GetKeyDown(eKeyCode::X))
		{
			AttackCount = 1;
		}

		Animation* mAnimation = mAnimator->GetAnimaiton();

		if (mAnimation->IsComplete())
		{
			if (AttackCount > 0)
			{
				AttackCount = 0;

				if (SecondAttack)
				{
					SecondAttack = false;


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
					SecondAttack = true;
					AttackCount = 0;


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
			}

			else
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

		}


	}
	void Skul::SkillA()
	{

	}
	void Skul::SkillS()
	{
	}

	void Skul::Jumpattack()
	{
		Rigidbody* rd = GetComponent<Rigidbody>();

		if (rd->GetGround())
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
			velocity.x = -4000;
			velocity.y = 0;
		}

		else if (mDirect == eDirection::Right)
		{
			velocity.x = 4000;
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
	}



	void Skul::StartSkillA()
	{

	}

	void Skul::CompleteSkillA()
	{
		if (mDirect == eDirection::Left)
			mAnimator->Play(L"LeftIdle", true);

		else if (mDirect == eDirection::Right)
			mAnimator->Play(L"RightIdle", true);

		mState = eSkulState::Idle;
	}

	void Skul::EndSkillA()
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

	void Skul::EndFall()
	{

	}



	void Skul::StartSkillS()
	{


	}

	void Skul::CompleteSkillS()
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

	void Skul::EndSkillS()
	{
	}
}