#include "yaChapter1_Boss.h"
#include "yaAnimator.h"
#include "yaResources.h"
#include "yaTransform.h"
#include "yaCamera.h"
#include "yaCollider.h"
#include "yaObject.h"
#include "yaBoss_Body.h"
#include "yaBoss_Hand.h"
#include "yaBoss_Head.h"
#include "yaBossBullet.h"
#include "yaBoss_Chin.h"
#include "yaTime.h"

namespace ya
{
	Chapter1_Boss::Chapter1_Boss()
	{
	}
	Chapter1_Boss::~Chapter1_Boss()
	{
	}
	void Chapter1_Boss::Initialize()
	{

		Transform* tr = GetComponent<Transform>();
		Vector2 bosspos = tr->GetPos();
		
		mBody = object::Instantiate<Boss_Body>(bosspos,eLayerType::Struct);
		mHead = object::Instantiate<Boss_Head>(bosspos,eLayerType::Monster);
		mLeftHand = object::Instantiate<Boss_Hand>(bosspos, eLayerType::Monster);
		mRightHand = object::Instantiate<Boss_RightHand>(bosspos,eLayerType::Monster);
		object::Instantiate<Boss_Chin>(bosspos,eLayerType::Monster);
		//object::Instantiate<BossBullet>(bosspos, eLayerType::Monster);

		//mHand[1]->SetHandDirection(eDirection::Right);
		mState = eBossState::Idle;

		mTime = 0;

		GameObject::Initialize();
	}
	void Chapter1_Boss::Update()
	{

		if (!(mprevState == mState))
		{
			mLeftHand->SetPlayed(false);
		}

		mprevState = mState;

		switch (mState)
		{
		case ya::Chapter1_Boss::eBossState::Idle:
			Idle();
			break;
		case ya::Chapter1_Boss::eBossState::Punch:
			Punch();
			break;
		case ya::Chapter1_Boss::eBossState::Smash:
			Smash();
			break;
		case ya::Chapter1_Boss::eBossState::RangeAttack:
			RangeAttack();
			break;
		case ya::Chapter1_Boss::eBossState::Dead:
			Dead();
			break;
		default:
			break;
		}

		GameObject::Update();
	}
	void Chapter1_Boss::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void Chapter1_Boss::Release()
	{
		GameObject::Release();

	}


	void Chapter1_Boss::Idle()
	{
		mTime += Time::DeltaTime();

		if (mTime >= 3)
		{
			UINT state = rand();
			state %= 3;
			
			mState = (eBossState)(state + 1);
			mTime = 0;
		}


	}
	void Chapter1_Boss::Punch()
	{
		mLeftHand->SetHandState(Boss_Hand::eHandState::Punch);

		if (mLeftHand->GetHandComplete())
			mState = eBossState::Idle;
	}
	void Chapter1_Boss::Smash()
	{
		mLeftHand->SetHandState(Boss_Hand::eHandState::Smash);
		if (mLeftHand->GetHandComplete())
			mState = eBossState::Idle;
	}
	void Chapter1_Boss::RangeAttack()
	{
		mLeftHand->SetHandState(Boss_Hand::eHandState::Down);

		if (mLeftHand->GetHandComplete())
			mState = eBossState::Idle;
	}
	void Chapter1_Boss::Dead()
	{
		mLeftHand->SetHandState(Boss_Hand::eHandState::Down);

	}
}
