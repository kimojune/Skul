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
	{
	}
	Skul::~Skul()
	{
	}

	void Skul::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetPos(Vector2(400.0f, 700.0f));
		tr->SetScale(Vector2(2.5f,2.5f));
		//tr->SetScale(Vector2(1.5f, 1.5f));

		mDirect = eSkulDirection::Left;

		Image* LeftImage = Resources::Load<Image>(L"LeftSkul", L"..\\Resources\\SkulLeft.bmp");
		Image* RightImage = Resources::Load<Image>(L"RightSkul", L"..\\Resources\\SkulRight.bmp");

		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimation(L"LeftIdle", LeftImage, Vector2(2480.0f, 0.0f), -20, 20, 4, Vector2::Zero, 0.1);
		mAnimator->CreateAnimation(L"LeftRun", LeftImage, Vector2(2480.0f, 124.0f), -20, 20, 8, Vector2::Zero, 0.1);

		mAnimator->CreateAnimation(L"RightIdle", RightImage, Vector2(0.0f, 0.0f), 20, 20, 4, Vector2(-85.0f,-50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightRun", RightImage, Vector2(0.0f, 124.0f), 20, 20, 8, Vector2(-85.0f, -50.0f), 0.1);

		//mAnimator->CreateAnimation(L"FowardRight", mImage, Vector2(0.0f, 113.0f), 16, 8, 15, Vector2::Zero, 0.1);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Idle", Vector2::Zero, 0.1f);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Aim\\Straight", Vector2::Zero, 0.1f);

		mAnimator->GetStartEvent(L"RightIdle") = std::bind(&Skul::idleCompleteEvent, this);
		mAnimator->Play(L"LeftIdle", true);
		//
		Collider* collider = AddComponent<Collider>();
		collider->SetCenter(Vector2(-20.0f, -25.0f));
		collider->SetSize(Vector2(50.0f, 50.0f));

		mState = eSkulState::Idle;

		GameObject::Initialize();
	}

	void Skul::Update()
	{
		GameObject::Update();

		switch (mState)
		{
		case ya::Skul::eSkulState::Idle:
			idle();
			break;
		case ya::Skul::eSkulState::Move:
			move();
			break;
		case ya::Skul::eSkulState::Dash:
			dash();
			break;
		case ya::Skul::eSkulState::Jump:
			jump();
		case ya::Skul::eSkulState::Fall:
			fall();
		case ya::Skul::eSkulState::FallRepeat:
			fallrepeat();		
		case ya::Skul::eSkulState::AttackA:
			attackA();
		case ya::Skul::eSkulState::AttackB:
			attackB();
		case ya::Skul::eSkulState::JumpAttack:
			jumpattack();
		case ya::Skul::eSkulState::Skill:
			skill();
			break;
		default:
			break;
		}
	}

	void Skul::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void Skul::Release()
	{
		GameObject::Release();

	}
	void Skul::move()
	{
		if (Input::GetKeyNone(eKeyCode::LEFT)
			&& Input::GetKeyNone(eKeyCode::RIGHT)
			&& Input::GetKeyNone(eKeyCode::DOWN)
			&& Input::GetKeyNone(eKeyCode::UP))
		{
			switch(mDirect)
			{
				case ya::Skul::eSkulDirection::Left:
				{
					mState = eSkulState::Idle;
					mAnimator->Play(L"LeftIdle", true);
				}
				case ya::Skul::eSkulDirection::Right:
				{
					mState = eSkulState::Idle;
					mAnimator->Play(L"RightIdle", true);
				}
			}

		}
	
		if (Input::GetKeyDown(eKeyCode::LEFT)
			|| Input::GetKeyDown(eKeyCode::RIGHT)
			|| Input::GetKeyDown(eKeyCode::DOWN)
			|| Input::GetKeyDown(eKeyCode::UP))
		{
			mState = eSkulState::Move;
			//mAnimator->Play(L"Idle", true);
		}

		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();
		
		if (Input::GetKey(eKeyCode::LEFT))
			pos.x -= 100.0f * Time::DeltaTime();

		if (Input::GetKey(eKeyCode::RIGHT))
			pos.x += 100.0f * Time::DeltaTime();

		//if (Input::GetKey(eKeyCode::W))
		//	pos.y -= 100.0f * Time::DeltaTime();
		//
		//if (Input::GetKey(eKeyCode::S))
		//	pos.y += 100.0f * Time::DeltaTime();
		//
		tr->SetPos(pos);
	}
	void Skul::dash()
	{
	}
	void Skul::jump()
	{
	}
	void Skul::fall()
	{
	}
	void Skul::fallrepeat()
	{
	}
	void Skul::attackA()
	{
	}
	void Skul::attackB()
	{
	}

	void Skul::jumpattack()
	{
	}

	void Skul::skill()
	{
		Transform* tr = GetComponent<Transform>();
		if (Input::GetKey(eKeyCode::K))
		{
			Scene* curScene = SceneManager::GetActiveScene();
			BaseBullet* bullet = new BaseBullet();
			bullet->GetComponent<Transform>()->SetPos(tr->GetPos());
			curScene->AddGameObeject(bullet, eLayerType::Bullet);
		}
	}
	////void Skul::death()
	//{
	//}
	void Skul::idle()
	{
		if (Input::GetKeyDown(eKeyCode::LEFT)
			|| Input::GetKeyDown(eKeyCode::RIGHT)
			|| Input::GetKeyDown(eKeyCode::DOWN)
			|| Input::GetKeyDown(eKeyCode::UP)
			)

		{
			if (Input::GetKeyDown(eKeyCode::LEFT))
			{
				mDirect = eSkulDirection::Left;
				mState = eSkulState::Move;
				mAnimator->Play(L"LeftRun", true);
			}

			else if (Input::GetKeyDown(eKeyCode::RIGHT))
			{
				mDirect = eSkulDirection::Right;
				mState = eSkulState::Move;
				mAnimator->Play(L"RightRun", true);
			}
			//mAnimator->Play(L"RightRun", true);
				
		}

		if (Input::GetKeyDown(eKeyCode::K))
		{
			mState = eSkulState::Skill;
			mAnimator->Play(L"AimStraight", true);
		}
	}

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
