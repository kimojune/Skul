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
	{
	}
	Monster::~Monster()
	{
	}
	void Monster::Idle()
	{
	}
	void Monster::Move()
	{
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
	void Monster::Initialize()
	{
		//Goopy Le Grande\Phase 1\Jump
		Transform* tr = GetComponent<Transform>();
	
		tr->SetScale(Vector2(2.0f, 2.0f));

		mAnimator = AddComponent<Animator>();
		//mAnimator->CreateAnimations(L"..\\Resources\\Goopy Le Grande\\Phase 1\\Jump", Vector2::Zero, 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\Idle", Vector2(-14.0f,-8.0f), 0.1f);
		mAnimator->Play(L"EntIdle", true);

		Collider* collider = AddComponent<Collider>();
		collider->SetCenter(Vector2(-40.0f, -45.0f));

		mState = eMonsterState::Idle;

		GameObject::Initialize();
	}
	void Monster::Update()
	{
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
}