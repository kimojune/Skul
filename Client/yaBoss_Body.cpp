#include "yaBoss_Body.h"
#include "yaAnimator.h"
#include "yaResources.h"
#include "yaTransform.h"
#include "yaCollider.h"
#include "yaTime.h"
#include "yaChapter1_Boss.h"
#include "yaSkulAttack.h"

namespace ya
{
	Boss_Body::Boss_Body()
		: mBoss(nullptr)
		, mAnimator(nullptr)
		, mSpeed(0.0f)
		, mbArrived(true)
		, mLook(eLook::Normal)
	{
	}
	Boss_Body::~Boss_Body()
	{
	}
	void Boss_Body::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.5f, 2.5f));
		mHomePos = tr->GetPos();

		Image* image[3] = {};
		image[0] = Resources::Load<Image>(L"Boss_Body_1", L"..\\Resources\\Boss\\Body\\Boss_Body_1.bmp");
		image[1] = Resources::Load<Image>(L"Boss_Body_2", L"..\\Resources\\Boss\\Body\\Boss_Body_2.bmp");
		image[2] = Resources::Load<Image>(L"Boss_Body_3", L"..\\Resources\\Boss\\Body\\Boss_Body_3.bmp");

		mAnimator = AddComponent<Animator>();
		mAnimator->CreateAnimation(L"BossBody_1", image[0], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1f);
		mAnimator->CreateAnimation(L"BossBody_2", image[1], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1f);
		mAnimator->CreateAnimation(L"BossBody_3", image[2], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1f);
		mAnimator->Play(L"BossBody_1", false);

		// 그로기 때 맞는 범위: 몸통 가운데 (주저앉으면 바닥 높이까지 내려온다)
		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(700.0f, 380.0f));
		collider->SetCenter(Vector2(-75.0f, 0.0f));

		GameObject::Initialize();
	}
	void Boss_Body::Update()
	{
		if (!mbArrived)
			mbArrived = BossMoveToward(mOffset, mTargetOffset, mSpeed > 0.0f ? mSpeed * (float)Time::DeltaTime() : -1.0f);

		GetComponent<Transform>()->SetPos(mHomePos + mOffset);

		GameObject::Update();
	}
	void Boss_Body::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void Boss_Body::Release()
	{
	}

	void Boss_Body::OnCollisionEnter(Collider* other)
	{
		OnCollisionStay(other);
	}
	void Boss_Body::OnCollisionStay(Collider* other)
	{
		if (mBoss == nullptr)
			return;
		if (dynamic_cast<SkulAttack*>(other->GetOwner()) == nullptr)
			return;

		mBoss->OnPartAttacked(Chapter1_Boss::ePart::Body);
	}
	void Boss_Body::OnCollisionExit(Collider* other)
	{
	}

	void Boss_Body::SetOffset(Vector2 offset, float speed)
	{
		mTargetOffset = offset;
		mSpeed = speed;
		mbArrived = false;
	}

	void Boss_Body::SnapOffset(Vector2 offset)
	{
		mOffset = offset;
		mTargetOffset = offset;
		mbArrived = true;
		GetComponent<Transform>()->SetPos(mHomePos + mOffset);
	}

	void Boss_Body::SetLook(eLook look)
	{
		if (mLook == look)
			return;

		mLook = look;
		switch (look)
		{
		case eLook::Phase2:
			mAnimator->Play(L"BossBody_2", false);
			break;
		case eLook::Flash:
			mAnimator->Play(L"BossBody_3", false);
			break;
		default:
			mAnimator->Play(L"BossBody_1", false);
			break;
		}
	}
}
