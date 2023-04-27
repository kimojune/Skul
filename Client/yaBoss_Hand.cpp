#include "yaBoss_Hand.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaResources.h"
#include "yaSceneManager.h"
#include "yaPlayeScene.h"
#include "yaSkul.h"
#include "yaTime.h"

namespace ya
{
	Boss_Hand::Boss_Hand()
		:mDirection(eDirection::Left)
	{
	}
	Boss_Hand::Boss_Hand(bool isRight)
	{
		mDirection = (eDirection)isRight;
	}
	Boss_Hand::~Boss_Hand()
	{
	}
	void Boss_Hand::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.5f, 2.5f));
	
		Vector2 handpos = tr->GetPos();

		handpos.y += 200.0f;
		
		if(mDirection==eDirection::Left)
			handpos.x -= 200.0f;
		else
			handpos.x += 200.0f;

		tr->SetPos(handpos);


		mImage[0] = Resources::Load<Image>(L"Boss_LeftHand", L"..\\Resources\\Boss\\Hand\\Boss_Left_Hand.bmp");
		mImage[1] = Resources::Load<Image>(L"Boss_RightHand", L"..\\Resources\\Boss\\Hand\\Boss_Right_Hand.bmp");
		
		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimation(L"Boss_LeftHand", mImage[0], Vector2(0, 0), 1, 5, 1, 5, Vector2(-84.0f,-76.0f), 0.3);
		mAnimator->CreateAnimation(L"Boss_RightHand", mImage[1], Vector2(672.0f, 0.0f), -1, 5, 1, 5, Vector2(-84.0f, -76.0f), 0.3);

		if (mDirection == eDirection::Left)
			mAnimator->Play(L"Boss_RightHand", true);
		else 
			mAnimator->Play(L"Boss_RightHand", true);


		GameObject::Initialize();
		
	}
	void Boss_Hand::Update()
	{

		switch (mHandState)
		{
		case ya::Boss_Hand::eHandState::Idle:
			Idle();
			break;
		case ya::Boss_Hand::eHandState::Down:
			Down();
			break;
		case ya::Boss_Hand::eHandState::Punch:
			Punch();
			break;
		case ya::Boss_Hand::eHandState::Smash:
			Smash();
			break;
		default:
			break;
		}

		GameObject::Update();

	}
	void Boss_Hand::Render(HDC hdc)
	{
		GameObject::Render(hdc);

	}
	void Boss_Hand::Release()
	{
		GameObject::Release();

	}
	void Boss_Hand::Idle()
	{
	}
	void Boss_Hand::Down()
	{
	}
	void Boss_Hand::Punch()
	{
		PlayeScene* playscene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
		Skul* skul = playscene->GetSkul();
		Transform* skultr = skul->GetComponent<Transform>();
		Transform* tr = GetComponent<Transform>();

		
		Vector2 skulpos = skultr->GetPos();
		Vector2 handpos = tr->GetPos();
		
		handpos += (skulpos - handpos).Normalize() * 100 * Time::DeltaTime();
		tr->SetPos(handpos);
	}
	void Boss_Hand::Smash()
	{
	}
}