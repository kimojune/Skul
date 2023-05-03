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
#include "yaPlayeScene.h"
#include "yaSkul.h"
#include "yaPlatform.h"

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
		
		mBody = object::Instantiate<Boss_Body>(bosspos,eLayerType::Boss);
		mHead = object::Instantiate<Boss_Head>(bosspos,eLayerType::Monster);
		mChin = object::Instantiate<Boss_Chin>(bosspos,eLayerType::Boss);
		mLeftHand = object::Instantiate<Boss_Hand>(bosspos, eLayerType::Monster);
		mRightHand = object::Instantiate<Boss_RightHand>(bosspos,eLayerType::Monster);

		mPlatform[0] = object::Instantiate<Platform>(Vector2(bosspos.x - 50.0f,bosspos.y+180.0f), eLayerType::Ground);
		mPlatform[1] = object::Instantiate<Platform>(Vector2(bosspos.x + 350.0f, bosspos.y + 100.0f), eLayerType::Ground);
		
		for (size_t i = 0; i < 8; i++)
		{
			mBullet[i] = object::Instantiate<BossBullet>(bosspos, eLayerType::Bullet);
			mBullet[i]->SetDirect((eDirection)i);
			mBullet[i]->SetState(eState::Pause);
		}

		
		mHP = 50;

		mLeftPos = Vector2(bosspos.x - 450, bosspos.y);
		mRightPos = Vector2(bosspos.x + 450, bosspos.y);

		mbodytr = mBody->GetComponent<Transform>();
		mheadtr = mHead->GetComponent<Transform>();
		mchintr = mChin->GetComponent<Transform>();
		//mHand[1]->SetHandDirection(eDirection::Right);
		mState = eBossState::Idle;

		mTime = 0;

		GameObject::Initialize();
	}
	void Chapter1_Boss::Update()
	{

		if (!(mprevState == mState))
		{
			PlayeScene* playscene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
			Skul* skul = playscene->GetSkul();
			if (skul == nullptr)
				return;
			Transform* skultr = skul->GetComponent<Transform>();
			mLeftHand->SetTargetPos(skultr->GetPos());
			mLeftHand->SetPlayed(false);
			mBody->SetPlayed(false);
			mHead->SetPlayed(false);
			mChin->SetOpen(false);
			//mPlatform[0]->SetState(eState::Active);
			//mPlatform[1]->SetState(eState::Active);

			if (mState == eBossState::RangeAttack)
			{
				for (size_t i = 0; i < 8; i++)
				{
					mBullet[i]->SetPrevPos();
					mBullet[i]->SetmPlay(false);
					mBullet[i]->SetState(eState::Active);
					
				}
			}
			
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

		if (mTime >= 5)
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
		
		//mPlatform[0]->SetState(eState::Pause);
		//mPlatform[1]->SetState(eState::Pause);

		if (mLeftHand->GetHandComplete())
		{
			mTime += Time::DeltaTime();

			if (mTime > 2)
			{
				mState = eBossState::Idle;
				mLeftHand->SetHandState(Boss_Hand::eHandState::Idle);
				mTime = 0;
			}
		}
	}
	void Chapter1_Boss::Smash()
	{
		mLeftHand->SetHandState(Boss_Hand::eHandState::Smash);
		mHead->SetHeadState(Boss_Head::eHeadState::Left);
		mBody->SetBodyState(Boss_Body::eBodyState::Left);
		//mPlatform[0]->SetState(eState::Pause);
		//mPlatform[1]->SetState(eState::Pause);

		if (mLeftHand->GetHandComplete())
		{
			mTime += Time::DeltaTime();
			
			if (mTime > 2)
			{
				mState = eBossState::Idle;
				mLeftHand->SetHandState(Boss_Hand::eHandState::Idle);
				mBody->SetBodyState(Boss_Body::eBodyState::Idle);
				mHead->SetHeadState(Boss_Head::eHeadState::Idle);

				mTime = 0;
			}
		}
	}
	void Chapter1_Boss::RangeAttack()
	{
		mLeftHand->SetHandState(Boss_Hand::eHandState::Down);
		mChin->SetOpen(true);
		



		mTime += Time::DeltaTime();

		if (mTime > 6)
		{
			mState = eBossState::Idle;
			mLeftHand->SetHandState(Boss_Hand::eHandState::Idle);
			mTime = 0;


		}
		//if (mLeftHand->GetHandComplete())
		//{
		//	mTime += Time::DeltaTime();

		//	if (mTime > 3)
		//	{
		//		mState = eBossState::Idle;
		//		mLeftHand->SetHandState(Boss_Hand::eHandState::Idle);
		//		mTime = 0;
		//	}
		//}
	}
	void Chapter1_Boss::Dead()
	{
		mLeftHand->SetHandState(Boss_Hand::eHandState::Down);
		mBody->SetBodyState(Boss_Body::eBodyState::Down);
		mHead->SetHeadState(Boss_Head::eHeadState::Down);
		//mPlatform[0]->SetState(eState::Pause);
		//mPlatform[1]->SetState(eState::Pause);


	}
}
