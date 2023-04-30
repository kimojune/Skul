#include "yaBoss_RightHand.h"
#include "yaBoss_Hand.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaResources.h"
#include "yaSceneManager.h"
#include "yaPlayeScene.h"
#include "yaSkul.h"
#include "yaTime.h"
#include "yaCollider.h"
#include "yaChapter1_Boss.h"

namespace ya
{
	Boss_RightHand::Boss_RightHand()
		:mDirection(eDirection::Right)
	{
	}

	Boss_RightHand::~Boss_RightHand()
	{
	}
	void Boss_RightHand::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.5f, 2.5f));

		Vector2 handpos = tr->GetPos();

		handpos.y += 200.0f;

		if (mDirection == eDirection::Left)
			handpos.x -= 250.0f;
		else
			handpos.x += 650.0f;

		tr->SetPos(handpos);
		mPrevPos = handpos;


		mRightImage[0] = Resources::Load<Image>(L"Boss_RightHand", L"..\\Resources\\Boss\\Hand\\Boss_Right_Hand.bmp");
		mRightImage[1] = Resources::Load<Image>(L"Boss_RightPunch", L"..\\Resources\\Boss\\Hand\\Punch_1_Right.bmp");
		mRightImage[2] = Resources::Load<Image>(L"Boss_RightPutOn", L"..\\Resources\\Boss\\Hand\\PutOn_1_Right.bmp");
		mRightImage[3] = Resources::Load<Image>(L"Boss_RightSmesh", L"..\\Resources\\Boss\\Hand\\Smesh_1_Right.bmp");


		mAnimator = AddComponent<Animator>();


		mAnimator->CreateAnimation(L"Boss_RightHand", mRightImage[0], Vector2(672.0f, 0.0f), -1, 5, 1, 5, Vector2(-84.0f, -76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_RightPunch", mRightImage[1], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_RightPutOn", mRightImage[2], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_RightSmesh", mRightImage[3], Vector2(0, 0), 1, 1, 1, 1, Vector2(-84.0f, -76.0f), 0.3);


		mAnimator->GetStartEvent(L"Boss_RightPutOn") = std::bind(&Boss_RightHand::StartDown, this);

		if (mDirection == eDirection::Left)
			mAnimator->Play(L"Boss_RightHand", true);
		else
			mAnimator->Play(L"Boss_RightHand", true);


		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(250.0f, 250.0f));
		collider->SetCenter(Vector2(-50.0f, -125.0f));

		mHandState = eHandState::Down;
		mAnimator->Play(L"Boss_RightPutOn", true);

		Scene* scene = SceneManager::GetActiveScene();
		mBoss = dynamic_cast<Chapter1_Boss*>(scene->GetGameObjects(eLayerType::Monster)[0]);

		GameObject::Initialize();

	}
	void Boss_RightHand::Update()
	{


		switch (mHandState)
		{
		case ya::Boss_RightHand::eHandState::Idle:
			Idle();
			break;
		case ya::Boss_RightHand::eHandState::Down:
			Down();
			break;
		case ya::Boss_RightHand::eHandState::Punch:
			Punch();
			break;
		case ya::Boss_RightHand::eHandState::Smash:
			Smash();
			break;
		default:
			break;
		}

		GameObject::Update();

	}
	void Boss_RightHand::Render(HDC hdc)
	{
		GameObject::Render(hdc);

	}
	void Boss_RightHand::Release()
	{
		GameObject::Release();

	}

	void Boss_RightHand::Idle()
	{
		Chapter1_Boss::eBossState bossstate = mBoss->GetBossState();

		switch (bossstate)
		{
		case ya::Chapter1_Boss::eBossState::Idle:
			break;
		case ya::Chapter1_Boss::eBossState::Punch:
			mHandState = eHandState::Punch;
			mAnimator->Play(L"Boss_RightPunch", true);
			break;
		case ya::Chapter1_Boss::eBossState::Smash:
			mHandState = eHandState::Smash;
			mAnimator->Play(L"Boss_RightSmash", true);

			break;
		case ya::Chapter1_Boss::eBossState::RangeAttack:
			mHandState = eHandState::Down;
			mAnimator->Play(L"Boss_RightPutOn", true);
			break;
		case ya::Chapter1_Boss::eBossState::Dead:
			mHandState = eHandState::Down;
			mAnimator->Play(L"Boss_RightPutOn", true);
			break;
		case ya::Chapter1_Boss::eBossState::End:
			break;
		default:
			break;
		}
	}
	void Boss_RightHand::Down()
	{
	}

	void Boss_RightHand::Punch()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 handpos = tr->GetPos();
		handpos += (mTargetPos - handpos).Normalize() * 3000 * Time::DeltaTime();
		tr->SetPos(handpos);

		if (handpos == mTargetPos)
		{
			mHandState = eHandState::Idle;
			mAnimator->Play(L"Boss_RightHand", true);
		}
		//mTargetPos = skultr->GetPos();
	}
	void Boss_RightHand::Smash()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 handpos = tr->GetPos();
		handpos.x = 0.0f;

	}

	void Boss_RightHand::StartDown()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 handpos = tr->GetPos();

		handpos.y += 200.0f;

		tr->SetPos(handpos);
	}
	void Boss_RightHand::StartPunch()
	{
		mHandState = eHandState::Punch;
		PlayeScene* playscene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
		Skul* skul = playscene->GetSkul();
		Transform* skultr = skul->GetComponent<Transform>();

		mTargetPos = skultr->GetPos();

	}
	void Boss_RightHand::CompletePunch()
	{

	}
	void Boss_RightHand::EndPunch()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetPos(mPrevPos);
	}

	void Boss_RightHand::StartSmash()
	{
	}

	void Boss_RightHand::CompleteSmash()
	{
	}

	void Boss_RightHand::EndSmash()
	{
	}
}