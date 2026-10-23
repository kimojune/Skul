#include "yaBoss_Hand.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaResources.h"
#include "yaImage.h"
#include "yaTime.h"
#include "yaCollider.h"
#include "yaChapter1_Boss.h"
#include "yaSkul.h"
#include "yaSkulAttack.h"
#include "yaPlayerStats.h"

namespace ya
{
	// 손 그림은 2.5배로 그린다. Animation::Render는 (위치 + offset)을 "확대 전" 그림의 아래쪽 가운데로 보고
	// 왼쪽 위에서부터 확대해 그리므로, 확대 후 그림의 아래쪽 가운데가 위치에 오도록 offset을 계산한다.
	static const float BossHandScale = 2.5f;

	static Vector2 BossHandOffset(Image* image, int frames)
	{
		float width = (float)image->GetWidth() / (float)frames;
		float height = (float)image->GetHeight();
		return Vector2(-width / 2.0f * (BossHandScale - 1.0f), -height * (BossHandScale - 1.0f));
	}

	static Image* BossHandLoad(const wchar_t* path)
	{
		// 다른 폴더에 같은 파일명이 있을 수 있어 경로 전체를 키로 쓴다
		return Resources::Load<Image>(path, path);
	}

	Boss_Hand::Boss_Hand()
		: Boss_Hand(false)
	{
	}
	Boss_Hand::Boss_Hand(bool isRight)
		: mBoss(nullptr)
		, mAnimator(nullptr)
		, mCollider(nullptr)
		, mbRight(isRight)
		, mbPhase2(false)
		, mPose(ePose::Hover)
		, mSpeed(0.0f)
		, mbArrived(true)
		, mbTremble(false)
		, mTime(0.0f)
		, mDamage(0)
		, mbHitDone(false)
		, mLinked(nullptr)
	{
	}
	Boss_Hand::~Boss_Hand()
	{
	}
	void Boss_Hand::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(BossHandScale, BossHandScale));
		mPos = tr->GetPos();
		mTarget = mPos;

		mAnimator = AddComponent<Animator>();

		// 1페이즈 그림. 대기는 5장짜리 시트이고, 오른손 시트는 왼손을 뒤집은 것이라 오른쪽 끝에서부터 읽는다
		Image* hover = BossHandLoad(mbRight ? L"..\\Resources\\Boss\\Hand\\Boss_Right_Hand.bmp" : L"..\\Resources\\Boss\\Hand\\Boss_Left_Hand.bmp");
		Image* fist = BossHandLoad(mbRight ? L"..\\Resources\\Boss\\Hand\\Punch_1_Right.bmp" : L"..\\Resources\\Boss\\Hand\\Punch_1.bmp");
		Image* flat = BossHandLoad(mbRight ? L"..\\Resources\\Boss\\Hand\\PutOn_1_Right.bmp" : L"..\\Resources\\Boss\\Hand\\PutOn_1.bmp");
		Image* sweep = BossHandLoad(mbRight ? L"..\\Resources\\Boss\\Hand\\Smesh_1_Right.bmp" : L"..\\Resources\\Boss\\Hand\\Smesh_1.bmp");

		float hoverCell = (float)hover->GetWidth() / 5.0f;
		Vector2 hoverStart = mbRight ? Vector2(hoverCell * 4.0f, 0.0f) : Vector2::Zero;
		mAnimator->CreateAnimation(L"Hover", hover, hoverStart, mbRight ? -1 : 1, 5, 1, 5, BossHandOffset(hover, 5), 0.3f);
		mAnimator->CreateAnimation(L"Fist", fist, Vector2::Zero, 1, 1, 1, 1, BossHandOffset(fist, 1), 0.3f);
		mAnimator->CreateAnimation(L"Flat", flat, Vector2::Zero, 1, 1, 1, 1, BossHandOffset(flat, 1), 0.3f);
		mAnimator->CreateAnimation(L"Sweep", sweep, Vector2::Zero, 1, 1, 1, 1, BossHandOffset(sweep, 1), 0.3f);

		// 2페이즈 보라색 그림(_2). 오른손 대기·바닥 짚기는 보라색 그림이 없어 1페이즈 그림을 그대로 쓴다
		// (렌더러가 색 곱하기를 지원하면 Chapter1_Boss::Phase2Tint 로 칠할 자리)
		Image* fist2 = BossHandLoad(mbRight ? L"..\\Resources\\Boss\\Hand\\Punch_2_Right.bmp" : L"..\\Resources\\Boss\\Hand\\Punch_2.bmp");
		Image* sweep2 = BossHandLoad(mbRight ? L"..\\Resources\\Boss\\Hand\\Smesh_2_Right.bmp" : L"..\\Resources\\Boss\\Hand\\Smesh_2.bmp");
		mAnimator->CreateAnimation(L"Fist2", fist2, Vector2::Zero, 1, 1, 1, 1, BossHandOffset(fist2, 1), 0.3f);
		mAnimator->CreateAnimation(L"Sweep2", sweep2, Vector2::Zero, 1, 1, 1, 1, BossHandOffset(sweep2, 1), 0.3f);
		if (!mbRight)
		{
			Image* hover2 = BossHandLoad(L"..\\Resources\\Boss\\Hand\\Boss_Left_Hand_2.bmp");
			Image* flat2 = BossHandLoad(L"..\\Resources\\Boss\\Hand\\PutOn_2.bmp");
			mAnimator->CreateAnimation(L"Hover2", hover2, Vector2::Zero, 1, 5, 1, 5, BossHandOffset(hover2, 5), 0.3f);
			mAnimator->CreateAnimation(L"Flat2", flat2, Vector2::Zero, 1, 1, 1, 1, BossHandOffset(flat2, 1), 0.3f);
		}

		mCollider = AddComponent<Collider>();

		mPose = ePose::Hover;
		playPose();

		GameObject::Initialize();
	}
	void Boss_Hand::Update()
	{
		float dt = (float)Time::DeltaTime();
		mTime += dt;

		// 속도 0 이하는 바로 도착 (BossMoveToward는 maxStep < 0을 순간이동으로 본다)
		if (!mbArrived)
			mbArrived = BossMoveToward(mPos, mTarget, mSpeed > 0.0f ? mSpeed * dt : -1.0f);

		Vector2 drawPos = mPos;
		if (mbTremble)
		{
			drawPos.x += sinf(mTime * 55.0f) * 6.0f;
			drawPos.y += cosf(mTime * 41.0f) * 3.0f;
		}
		GetComponent<Transform>()->SetPos(drawPos);

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

	void Boss_Hand::OnCollisionEnter(Collider* other)
	{
		OnCollisionStay(other);
	}

	void Boss_Hand::OnCollisionStay(Collider* other)
	{
		GameObject* owner = other->GetOwner();

		// 플레이어 공격 판정과 겹침: 그로기일 때만 맞는다 (판단은 보스가 한다)
		if (dynamic_cast<SkulAttack*>(owner) != nullptr)
		{
			if (mBoss != nullptr)
				mBoss->OnPartAttacked(Chapter1_Boss::ePart::Hand);
			return;
		}

		if (mDamage <= 0 || mbHitDone)
			return;
		if (dynamic_cast<Skul*>(owner) == nullptr)
			return;

		// 대시 무적 중이면 피해가 들어가지 않고, 공격이 끝나기 전에 다시 닿으면 그때 맞는다
		Vector2 source = mPos;
		source.y -= 150.0f;
		if (PlayerStats::TakeDamage(mDamage, source))
		{
			mbHitDone = true;
			if (mLinked != nullptr)
				mLinked->mbHitDone = true;
		}
	}

	void Boss_Hand::OnCollisionExit(Collider* other)
	{
	}

	void Boss_Hand::SetPose(ePose pose)
	{
		if (mPose == pose)
			return;

		mPose = pose;
		playPose();
	}

	void Boss_Hand::SetPhase2(bool phase2)
	{
		if (mbPhase2 == phase2)
			return;

		mbPhase2 = phase2;
		playPose();
	}

	void Boss_Hand::MoveTo(Vector2 pos, float speed)
	{
		mTarget = pos;
		mSpeed = speed;
		mbArrived = false;
	}

	void Boss_Hand::Teleport(Vector2 pos)
	{
		mPos = pos;
		mTarget = pos;
		mbArrived = true;
		GetComponent<Transform>()->SetPos(pos);
	}

	void Boss_Hand::SetAttack(int damage, Boss_Hand* linked)
	{
		mDamage = damage;
		mbHitDone = false;
		mLinked = linked;
	}

	void Boss_Hand::playPose()
	{
		const wchar_t* name = L"Hover";
		bool loop = false;
		switch (mPose)
		{
		case ePose::Hover:
			name = mbPhase2 && !mbRight ? L"Hover2" : L"Hover";
			loop = true;
			break;
		case ePose::Fist:
			name = mbPhase2 ? L"Fist2" : L"Fist";
			break;
		case ePose::Flat:
			name = mbPhase2 && !mbRight ? L"Flat2" : L"Flat";
			break;
		case ePose::Sweep:
			name = mbPhase2 ? L"Sweep2" : L"Sweep";
			break;
		default:
			break;
		}
		mAnimator->Play(name, loop);
		updateCollider();
	}

	void Boss_Hand::updateCollider()
	{
		// 충돌 상자도 아래쪽 가운데 기준. 그림보다 조금 작게 잡는다
		Vector2 size = Vector2(300.0f, 300.0f);
		switch (mPose)
		{
		case ePose::Hover:
			size = Vector2(300.0f, 320.0f);
			break;
		case ePose::Fist:
			size = Vector2(300.0f, 300.0f);
			break;
		case ePose::Flat:
			size = Vector2(440.0f, 220.0f);
			break;
		case ePose::Sweep:
			// 바닥만 쓴다: 뿌리 발판 위나 점프한 플레이어에게는 닿지 않는 높이
			size = Vector2(360.0f, 180.0f);
			break;
		default:
			break;
		}
		mCollider->SetSize(size);
		mCollider->SetCenter(Vector2(-size.x / 2.0f, -size.y));
	}
}
