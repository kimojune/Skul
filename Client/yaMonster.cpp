#include "yaMonster.h"
#include "yaTime.h"
#include "yaSceneManager.h"
#include "yaInput.h"
#include "yaResources.h"
#include "yaTransform.h"
#include "yaAnimator.h"
#include "yaCollider.h"
#include "yaSkulHead.h"
#include "yaScene.h"


namespace ya
{
	Monster::Monster()
		:mTime(0)
		, mDirection(eDirection::Right)
		
	{
	}
	Monster::~Monster()
	{
	}
	void Monster::Initialize()
	{
		//Goopy Le Grande\Phase 1\Jump
		Transform* tr = GetComponent<Transform>();
	
		tr->SetScale(Vector2(2.0f, 2.0f));

		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\LeftIdle", Vector2(-14.0f,-8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\RightIdle", Vector2(-14.0f,-8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\LeftMove", Vector2(-14.0f,-8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\RightMove", Vector2(-14.0f,-8.0f), 0.1f);

		mAnimator->Play(L"EntRightMove", true);

		Collider* collider = AddComponent<Collider>();
		collider->SetCenter(Vector2(-40.0f, -45.0f));

		mState = eMonsterState::Idle;


		//mAnimator->GetStartEvent(L"LeftAttackA") = std::bind(&Monster::StartAttack, this);
		//mAnimator->GetCompleteEvent(L"LeftAttackA") = std::bind(&Skul::CompleteAttack, this);

		//mAnimator->GetEndEvent(L"EntLeftIdle") = std::bind(&Monster::EndIdle, this);
		//mAnimator->GetEndEvent(L"EntRightIdle") = std::bind(&Monster::EndIdle, this);
	
		GameObject::Initialize();


	}
	void Monster::Update()
	{
		mTime += Time::DeltaTime();

		if (mTime > 3)
		{
			mTime = 0;

			UINT rstate = rand();
			UINT rDir= rand();
			rstate %= 2;
			rDir %= 2;
			
			mDirection = (eDirection)rDir;


			mState = (eMonsterState)rstate;
			
			if (mState == eMonsterState::Idle)
			{
				if(mDirection == eDirection::Left)
				mAnimator->Play(L"EntLeftIdle", true);
				else
				mAnimator->Play(L"EntRightIdle", true);
			}

			else if (mState == eMonsterState::Move)
			{
				if (mDirection == eDirection::Left)
					mAnimator->Play(L"EntLeftMove", true);
				else
					mAnimator->Play(L"EntRightMove", true);
			}

			
		}


		switch (mState)
		{
		case ya::Monster::eMonsterState::Idle:
			Idle();
			break;
		case ya::Monster::eMonsterState::Move:
			Move();
			break;
		case ya::Monster::eMonsterState::Attack:
			Attack();
			break;
		case ya::Monster::eMonsterState::LangeAttack:
			LangeAttack();
			break;
		case ya::Monster::eMonsterState::Hit:
			Hit();
			break;
		case ya::Monster::eMonsterState::Dead:
			Dead();
			break;
		default:
			break;
		}

		GameObject::Update();
	}
	void Monster::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void Monster::Release()
	{
		GameObject::Release();
	}

	void Monster::Idle()
	{

	}
	void Monster::Move()
	{
		
		//�̵���

		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();

		if (mDirection == eDirection::Left)
		{
			pos.x -= 100.0f * Time::DeltaTime();
			if (pos.x < 0)
				pos.x = 0;
		}

		else if (mDirection == eDirection::Right)
		{
			pos.x += 100.0f * Time::DeltaTime();
		}

		tr->SetPos(pos);

	}
	void Monster::Attack()
	{

	}
	void Monster::LangeAttack()
	{

	}
	void Monster::Hit()
	{
	}
	void Monster::Dead()
	{
		
	}

	void Monster::CompleteIdle()
	{
	}

	void Monster::EndIdle()
	{
	}
}