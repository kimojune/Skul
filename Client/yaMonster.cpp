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
		//mAnimator->CreateAnimations(L"..\\Resources\\Goopy Le Grande\\Phase 1\\Jump", Vector2::Zero, 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\Idle", Vector2(-14.0f,-8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\Move", Vector2(-14.0f,-8.0f), 0.1f);
		mAnimator->Play(L"EntIdle", true);

		Collider* collider = AddComponent<Collider>();
		collider->SetCenter(Vector2(-40.0f, -45.0f));

		mState = eMonsterState::Idle;


		//mAnimator->GetStartEvent(L"LeftAttackA") = std::bind(&Monster::StartAttack, this);
		//mAnimator->GetCompleteEvent(L"LeftAttackA") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"EntIdle") = std::bind(&Monster::EndIdle, this);
	
		GameObject::Initialize();


	}
	void Monster::Update()
	{
		mTime += Time::DeltaTime();

		if (mTime > 8)
		{
			mTime = 0;

			UINT a = rand();
			a %= 2;
			mState = (eMonsterState)a;
			
			if (mState == eMonsterState::Idle)
				mAnimator->Play(L"EntIdle", true);
			else if(mState==eMonsterState::Move)
				mAnimator->Play(L"EntMove", true);

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
		

		//¿Ãµø∫Œ

		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();

		if (mDirection == eDirection::Left)
		{
			pos.x -= 100.0f * Time::DeltaTime();
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