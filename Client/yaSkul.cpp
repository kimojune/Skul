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
		mAnimator->CreateAnimation(L"LeftSkillA", LeftImage, Vector2(2356.0f, (124.0f * 9)), -1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.1);

		mAnimator->CreateAnimation(L"RightIdle", RightImage, Vector2(0.0f, 0.0f), 1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightRun", RightImage, Vector2(0.0f, 124.0f), 1, 20, 20, 8, Vector2(-85.0f, -50.0f), 0.08);
		mAnimator->CreateAnimation(L"RightSkillA", RightImage, Vector2(0.0f, (124.0f * 9)), 1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.1);


		//mAnimator->CreateAnimation(L"FowardRight", mImage, Vector2(0.0f, 113.0f), 16, 8, 15, Vector2::Zero, 0.1);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Idle", Vector2::Zero, 0.1f);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Aim\\Straight", Vector2::Zero, 0.1f);

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


		mState = eSkulState::Idle;



		GameObject::Initialize();
	}


	void Skul::Update()
	{
		if (Input::GetKeyDown(eKeyCode::LEFT) )
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
		case ya::Skul::eSkulState::Fall:
			Fall();
		case ya::Skul::eSkulState::FallRepeat:
			Fallrepeat();		
		case ya::Skul::eSkulState::AttackA:
			AttackA();
		case ya::Skul::eSkulState::AttackB:
			AttackB();
		case ya::Skul::eSkulState::JumpAttack:
			Jumpattack();
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

		if (Input::GetKeyDown(eKeyCode::A))
		{
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
		}

		//이동 부
		if (mState==eSkulState::Move)
		{
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();
		
		if (Input::GetKey(eKeyCode::LEFT))
		{
			pos.x -= 200.0f * Time::DeltaTime();
		}

		if (Input::GetKey(eKeyCode::RIGHT))
		{
			pos.x += 200.0f * Time::DeltaTime();

		}
		tr->SetPos(pos);

		}
	}

	void Skul::Dash()
	{
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
	void Skul::AttackA()
	{
	}
	void Skul::AttackB()
	{
	}

	void Skul::SkillA()
	{
		switch (mDirect)
		{
		case eDirection::Left:
		{
			mState = eSkulState::Idle;
			mAnimator->Play(L"LeftIdle", true);
			break;
		}
		case eDirection::Right:
		{
			mState = eSkulState::Idle;
			mAnimator->Play(L"RightIdle", true);
			break;
		}
		}

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
}
