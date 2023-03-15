#include "yaSkul.h"
#include "yaTime.h"
#include "yaSceneManager.h"
#include "yaInput.h"
#include "yaResources.h"
#include "yaTransform.h"
#include "yaAnimator.h"
#include "yaCollider.h"
#include "yaBaseBullet.h"
#include "yaScene.h"

namespace ya
{
	Skul::Skul()
		: mState(eSkulState::Idle)
	{
	}
	Skul::~Skul()
	{
	}

	void Skul::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetPos(Vector2(400.0f, 725.0f));
		tr->SetScale(Vector2(2.5f,2.5f));
		//tr->SetScale(Vector2(1.5f, 1.5f));

		mDirect = eSkulDirection::Right;

		Image* LeftImage = Resources::Load<Image>(L"LeftSkul", L"..\\Resources\\SkulLeft.bmp");
		Image* RightImage = Resources::Load<Image>(L"RightSkul", L"..\\Resources\\SkulRight.bmp");

		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimation(L"LeftIdle", LeftImage, Vector2(2356.0f, 0.0f),-1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftRun", LeftImage, Vector2(2356.0f, 124.0f),-1, 20, 20, 8, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"LeftSkill", LeftImage, Vector2(2356.0f, (124.0f * 9)),-1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.05);

		mAnimator->CreateAnimation(L"RightIdle", RightImage, Vector2(0.0f, 0.0f),1, 20, 20, 4, Vector2(-85.0f,-50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightRun", RightImage, Vector2(0.0f, 124.0f),1, 20, 20, 8, Vector2(-85.0f, -50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightSkill", RightImage, Vector2(0.0f, (124.0f* 9)),1, 20, 20, 4, Vector2(-85.0f, -50.0f), 0.05);

		//mAnimator->CreateAnimation(L"FowardRight", mImage, Vector2(0.0f, 113.0f), 16, 8, 15, Vector2::Zero, 0.1);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Idle", Vector2::Zero, 0.1f);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Aim\\Straight", Vector2::Zero, 0.1f);

		//mAnimator->GetStartEvent(L"RightIdle") = std::bind(&Skul::idleCompleteEvent, this);
		mAnimator->Play(L"RightIdle", true);
		//
		Collider* collider = AddComponent<Collider>();
		collider->SetCenter(Vector2(-20.0f, -25.0f));
		collider->SetSize(Vector2(50.0f, 50.0f));

		mState = eSkulState::Idle;

		GameObject::Initialize();
	}

	void Skul::Update()
	{
	

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
		case ya::Skul::eSkulState::Skill:
			Skill();
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

	void Skul::Idle()
	{
		if (Input::GetKeyDown(eKeyCode::LEFT)
			|| Input::GetKeyDown(eKeyCode::RIGHT))
			{
				mState = eSkulState::Move;
			if (Input::GetKeyDown(eKeyCode::LEFT))
			{
				mDirect = eSkulDirection::Left;
				mAnimator->Play(L"LeftRun", true);
			}

			if (Input::GetKeyDown(eKeyCode::RIGHT))
			{
				mDirect = eSkulDirection::Right;
				mAnimator->Play(L"RightRun", true);
			}
		}

		if (Input::GetKeyDown(eKeyCode::A))
		{
			mState = eSkulState::Skill;
		
			switch (mDirect)
			{
			case ya::Skul::eSkulDirection::Left:
				mAnimator->Play(L"LeftSkill", true);
				mState=eSkulState::Skill;
				break;

			case ya::Skul::eSkulDirection::Right:
				mAnimator->Play(L"RightSkill", true);
				mState = eSkulState::Skill;
				break;

			default:
				break;

			} 
		}
	}
	void Skul::Move()
	{

		if (Input::GetKeyNone(eKeyCode::LEFT)
			&& Input::GetKeyNone(eKeyCode::RIGHT))
		{
			mState = eSkulState::Idle;

			switch (mDirect)
			{
			case ya::Skul::eSkulDirection::Left:
			{
				mAnimator->Play(L"LeftIdle", true);
				break;
			}
			case ya::Skul::eSkulDirection::Right:
			{
				mAnimator->Play(L"RightIdle", true);
				break;
			}
			}
		}

		if (Input::GetKeyDown(eKeyCode::LEFT)
			|| Input::GetKeyDown(eKeyCode::RIGHT))
		{
			if (Input::GetKeyDown(eKeyCode::LEFT))
			{
				mDirect = eSkulDirection::Left;
				mAnimator->Play(L"LeftRun", true);
			}

			if (Input::GetKeyDown(eKeyCode::RIGHT))
			{
				mDirect = eSkulDirection::Right;
				mAnimator->Play(L"RightRun", true);
			}
		}

		//¿Ãµø ∫Œ
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

	void Skul::Jumpattack()
	{
	}

	void Skul::Skill()
	{

		Transform* tr = GetComponent<Transform>();

		if (Input::GetKeyDown(eKeyCode::A))
		{
			Scene* curScene = SceneManager::GetActiveScene();
			BaseBullet* bullet = new BaseBullet();
			bullet->GetComponent<Transform>()->SetPos(tr->GetPos());
			curScene->AddGameObeject(bullet, eLayerType::Bullet);
		}
		if (Input::GetKeyNone(eKeyCode::A))
		{
			switch (mDirect)
			{
			case ya::Skul::eSkulDirection::Left:
			{
				mState = eSkulState::Idle;
				mAnimator->Play(L"LeftIdle", true);
				break;
			}
			case ya::Skul::eSkulDirection::Right:
			{
				mState = eSkulState::Idle;
				mAnimator->Play(L"RightIdle", true);
				break;
			}
			}
		}
	}
	////void Skul::death()
	//{
	//}

	void Skul::idleCompleteEvent(/*const Cuphead* this*/)
	{
		int a = 0;
		//mState =
		//Transform* tr = GetComponent<Transform>();
		//Scene* curScene = SceneManager::GetActiveScene();
		//BaseBullet* bullet = new BaseBullet();
		//bullet->GetComponent<Transform>()->SetPos(tr->GetPos());
		//curScene->AddGameObeject(bullet, eLayerType::Bullet);
	}
}
