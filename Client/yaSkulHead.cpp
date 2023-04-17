#include "yaSkulHead.h"
#include "yaTransform.h"
#include "yaTime.h"
#include "yaObject.h"
#include "yaCollider.h"
#include "yaCamera.h"
#include "yaResources.h"
#include "yaSkul.h"
#include "yaPlayeScene.h"
#include "yaAnimator.h"
#include "yaMonster.h"


namespace ya
{
	SkulHead::SkulHead()
		:mTime(0.0f)
		, mDirect(eDirection::Left)
	{
		SetName(L"SkulHead");

	}
	SkulHead::~SkulHead()
	{
	}
	void SkulHead::Initialize()
	{
		mLeftImage = Resources::Load<Image>(L"LeftSkulHead", L"..\\Resources\\LeftSkulHead.bmp");
		mRightImage = Resources::Load<Image>(L"RightSkulHead", L"..\\Resources\\SkulHead.bmp");
		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(45.0f, 45.0f));
		collider->SetCenter(Vector2(0.0f, -50.0f));
		SetState(eState::Pause);
		
		GameObject::Initialize();
	}
	void SkulHead::Update()
	{
		
		Transform* tr = GetComponent<Transform>();
		Vector2 dir = Vector2(500.0f, 500.0f);
		dir.Normalize();

		Vector2 pos = tr->GetPos();

		if(mDirect == eDirection::Left)
			pos.x -= 1500.0f * dir.x * Time::DeltaTime();

		else if (mDirect== eDirection::Right)
			pos.x += 1500.0f * dir.x * Time::DeltaTime();

		if (pos.x < 0.0f)
		{
			pos.x = 0.0f;
		}

		mPos = pos;
		tr->SetPos(mPos);		

		mTime += Time::DeltaTime();

		if (mTime > 8.0f)
		{
			mTime = 0.0f;
			
			SetState(eState::Pause);
			Skul* skul = mSkuls[(UINT)Skul::eSkulType::Nohead];
			if (skul->GetState() == eState::Pause)
				return;
			
			Skul* basic = skul->SwitchSkul(Skul::eSkulType::Basic);
			basic->SetSkulState(Skul::eSkulState::SkillS);
			
		}

		GameObject::Update();
	}

	void SkulHead::Render(HDC hdc)
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = Camera::CaluatePos(tr->GetPos());

		switch (mDirect)
		{
		case eDirection::Left:
			TransparentBlt(hdc
				, pos.x, pos.y - 50.0f
				, mLeftImage->GetWidth() * 2.8f, mLeftImage->GetHeight() * 2.8f
				, mLeftImage->GetHdc()
				, 0, 0
				, mLeftImage->GetWidth(), mLeftImage->GetHeight()
				, RGB(255, 0, 255));
			break;

		case eDirection::Right:
			TransparentBlt(hdc
				, pos.x, pos.y - 50.0f
				, mRightImage->GetWidth() * 2.8f, mRightImage->GetHeight() * 2.8f
				, mRightImage->GetHdc()
				, 0, 0
				, mRightImage->GetWidth(), mRightImage->GetHeight()
				, RGB(255, 0, 255));
			break;

		default:
			break;
		}



		GameObject::Render(hdc);
	}

	void SkulHead::Release()
	{
	}

	void SkulHead::OnCollisionEnter(Collider* other)
	{
		Monster* monster = dynamic_cast<Monster*>(other->GetOwner());

		if (monster == nullptr)
			return;

			monster->SetMonsterState(Monster::eMonsterState::Hit);
			Animator* monAnimator = monster->GetComponent<Animator>();
	
	}

	void SkulHead::OnCollisionStay(Collider* other)
	{
	}

	void SkulHead::OnCollisionExit(Collider* other)
	{
	}

}