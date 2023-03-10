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
		tr->SetPos(Vector2(400.0f, 400.0f));
		tr->SetScale(Vector2(2.5f,2.5f));
		//tr->SetScale(Vector2(1.5f, 1.5f));

		Image* mImage = Resources::Load<Image>(L"Skul", L"..\\Resources\\Skul.bmp");
		mAnimator = AddComponent<Animator>();
		mAnimator->CreateAnimation(L"Idle", mImage, Vector2(0.0f, 0.0f), 20, 20, 4, Vector2(-85.0f,-50.0f), 0.1);
		mAnimator->CreateAnimation(L"RightRun", mImage, Vector2(0.0f, 124.0f), 20, 20, 8, Vector2(0.0f,0.0f), 0.1);

		//mAnimator->CreateAnimation(L"FowardRun", mImage, Vector2::Zero, 16, 8, 16, Vector2::Zero, 0.1);
		//mAnimator->CreateAnimation(L"FowardRight", mImage, Vector2(0.0f, 113.0f), 16, 8, 15, Vector2::Zero, 0.1);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Idle", Vector2::Zero, 0.1f);
		//mAnimator->CreateAnimations(L"..\\Resources\\Chalise\\Aim\\Straight", Vector2::Zero, 0.1f);

		mAnimator->GetStartEvent(L"Idle") = std::bind(&Skul::idleCompleteEvent, this);
		mAnimator->Play(L"Idle", true);
		//
		Collider* collider = AddComponent<Collider>();
		collider->SetCenter(Vector2(-20.0f, -25.0f));
		collider->SetSize(Vector2(50.0f, 50.0f));

		mState = eCupheadState::Idle;

		GameObject::Initialize();
	}

	void Skul::Update()
	{
		GameObject::Update();

		switch (mState)
		{
		case ya::Skul::eCupheadState::Move:
			move();
			break;
		case ya::Skul::eCupheadState::Shoot:
			shoot();
			break;
		case ya::Skul::eCupheadState::Death:
			death();
			break;
		case ya::Skul::eCupheadState::Idle:
			idle();
			break;
		default:
			break;
		}

		/*Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();
		Animator* animator = GetComponent<Animator>();
		if (Input::GetKeyState(eKeyCode::A) == eKeyState::Pressed)
		{
			pos.x -= 100.0f * Time::DeltaTime();
		}

		if (Input::GetKeyState(eKeyCode::D) == eKeyState::Pressed)
		{
			pos.x += 100.0f * Time::DeltaTime();
		}

		if (Input::GetKeyState(eKeyCode::W) == eKeyState::Pressed)
		{
			pos.y -= 100.0f * Time::DeltaTime();
		}

		if (Input::GetKeyState(eKeyCode::W) == eKeyState::Down)
		{
			animator->Play(L"FowardRun", true);
		}
		if (Input::GetKeyState(eKeyCode::W) == eKeyState::Up)
		{
			animator->Play(L"Idle", true);
		}

		if (Input::GetKeyState(eKeyCode::S) == eKeyState::Pressed)
		{
			pos.y += 100.0f * Time::DeltaTime();
		}
		tr->SetPos(pos);*/
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
		if (Input::GetKeyUp(eKeyCode::A)
			|| Input::GetKeyUp(eKeyCode::D)
			|| Input::GetKeyUp(eKeyCode::S)
			|| Input::GetKeyUp(eKeyCode::W))
		{
			mState = eCupheadState::Idle;
			//mAnimator->Play(L"Idle", true);
		}

		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();
		
		if (Input::GetKey(eKeyCode::A))
			pos.x -= 100.0f * Time::DeltaTime();

		if (Input::GetKey(eKeyCode::D))
			pos.x += 100.0f * Time::DeltaTime();

		if (Input::GetKey(eKeyCode::W))
			pos.y -= 100.0f * Time::DeltaTime();
		
		if (Input::GetKey(eKeyCode::S))
			pos.y += 100.0f * Time::DeltaTime();
		
		tr->SetPos(pos);
	}
	void Skul::shoot()
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
	void Skul::death()
	{
	}
	void Skul::idle()
	{
		if (Input::GetKeyDown(eKeyCode::A)
			|| Input::GetKeyDown(eKeyCode::D)
			|| Input::GetKeyDown(eKeyCode::S)
			|| Input::GetKeyDown(eKeyCode::W))
		{
			mState = eCupheadState::Move;
			//mAnimator->Play(L"FowardRun", true);
		}

		if (Input::GetKeyDown(eKeyCode::K))
		{
			mState = eCupheadState::Shoot;
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
