#include "yaBoss_Head.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaResources.h"
#include "yaCollider.h"
#include "yaTime.h"
#include "yaChapter1_Boss.h"

namespace ya
{
	Boss_Head::Boss_Head()
		: mBoss(nullptr)
		, mAnimator(nullptr)
		, mSpeed(0.0f)
		, mbArrived(true)
		, mbBob(true)
		, mTime(0.0f)
		, mHitShake(0.0f)
		, mLook(eLook::Normal)
	{
	}
	Boss_Head::~Boss_Head()
	{
	}
	void Boss_Head::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 headPos = tr->GetPos();
		headPos.x += 100.0f;
		mHomePos = headPos;

		tr->SetPos(headPos);
		tr->SetScale(Vector2(2.5f, 2.5f));

		mAnimator = AddComponent<Animator>();

		Image* image[3] = {};
		image[0] = Resources::Load<Image>(L"Boss_Head_1", L"..\\Resources\\Boss\\Head\\Boss_Head_1.bmp");
		image[1] = Resources::Load<Image>(L"Boss_Head_2", L"..\\Resources\\Boss\\Head\\Boss_Head_2.bmp");
		image[2] = Resources::Load<Image>(L"Boss_Head_3", L"..\\Resources\\Boss\\Head\\Boss_Head_3.bmp");

		mAnimator->CreateAnimation(L"BossHead_1", image[0], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1f);
		mAnimator->CreateAnimation(L"BossHead_2", image[1], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1f);
		mAnimator->CreateAnimation(L"BossHead_3", image[2], Vector2(0, 0), 0, 1, 1, 1, Vector2(-85.0f, -50.0f), 0.1f);
		mAnimator->Play(L"BossHead_1", false);

		// 맞는 범위 아래 끝은 y 1585. 픽셀 지면(서 있는 발 y 1709)에서 땅 공격 판정 윗변이 1599라
		// 300 높이(아래 끝 1600)면 머리 밑에 서서 땅 공격만으로 1픽셀 겹쳐 맞았다. 머리는 점프·뿌리 발판으로 때린다
		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(300.0f, 285.0f));
		collider->SetCenter(Vector2(-25.0f, -100.0f));

		GameObject::Initialize();
	}
	void Boss_Head::Update()
	{
		float dt = (float)Time::DeltaTime();
		mTime += dt;

		if (!mbArrived)
			mbArrived = BossMoveToward(mOffset, mTargetOffset, mSpeed > 0.0f ? mSpeed * dt : -1.0f);

		Vector2 pos = mHomePos + mOffset;
		if (mbBob)
			pos.y += sinf(mTime * 2.5f) * 6.0f;
		if (mHitShake > 0.0f)
		{
			mHitShake -= dt;
			pos.x += sinf(mTime * 90.0f) * 5.0f;
		}
		GetComponent<Transform>()->SetPos(pos);

		GameObject::Update();
	}
	void Boss_Head::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void Boss_Head::Release()
	{
	}

	void Boss_Head::SetOffset(Vector2 offset, float speed)
	{
		mTargetOffset = offset;
		mSpeed = speed;
		mbArrived = false;
	}

	void Boss_Head::SnapOffset(Vector2 offset)
	{
		mOffset = offset;
		mTargetOffset = offset;
		mbArrived = true;
		GetComponent<Transform>()->SetPos(mHomePos + mOffset);
	}

	void Boss_Head::SetLook(eLook look)
	{
		if (mLook == look)
			return;

		mLook = look;
		switch (look)
		{
		case eLook::Phase2:
			mAnimator->Play(L"BossHead_3", false);
			break;
		case eLook::Sane:
			mAnimator->Play(L"BossHead_2", false);
			break;
		default:
			mAnimator->Play(L"BossHead_1", false);
			break;
		}
	}

	Vector2 Boss_Head::GetGemPos()
	{
		// Head_1 그림(245x170)에서 마석은 (129, 82) 근처. 2.5배로 그리고 offset(-85, -50)을 더한 위치
		Vector2 pos = GetComponent<Transform>()->GetPos();
		pos.x += 115.0f;
		pos.y -= 15.0f;
		return pos;
	}

	void Boss_Head::Hit()
	{
		if (mBoss == nullptr)
			return;

		// 피해가 들어갔을 때만 움찔한다 (등장·전환·사망 연출 중에는 무시)
		if (mBoss->OnHeadHit())
			mHitShake = 0.12f;
	}
}
