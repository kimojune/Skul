#include "yaBossBullet.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaCollider.h"
#include "yaTime.h"
#include "yaSceneManager.h"
#include "yaSkul.h"			// yaPlayeScene.h가 Skul을 쓰므로 먼저 (unity 빌드에서 순서가 섞인다)
#include "yaPlayeScene.h"
#include "yaPlayerStats.h"

namespace ya
{
	// 탄 애니메이션 세 폴더(Impact/Loop/Outro)의 가장 큰 프레임이 약 200x200이다.
	// Animator는 (위치)를 확대 전 그림의 아래쪽 가운데로 보고 왼쪽 위부터 확대해 그리므로,
	// 확대한 그림의 가운데가 mCenter에 오도록 위치를 옮겨 준다.
	static const float BossBulletCell = 200.0f;
	// 구체의 보이는 지름(배율 1 기준). 충돌 상자는 이보다 조금 작게
	static const float BossBulletHitSize = 90.0f;
	// 바닥(지면 충돌체 윗변 1750)에 닿으면 터진다
	static const float BossBulletFloorY = 1750.0f;

	BossBullet::BossBullet()
		: mAnimator(nullptr)
		, mCollider(nullptr)
		, mBulletState(eBulletState::Charge)
		, mSpeed(0.0f)
		, mScale(1.0f)
		, mHoming(0.0f)
		, mTime(0.0f)
		, mDamage(0)
	{
	}
	BossBullet::~BossBullet()
	{
	}
	void BossBullet::Initialize()
	{
		mCenter = GetComponent<Transform>()->GetPos();

		mAnimator = AddComponent<Animator>();
		mAnimator->CreateAnimations(L"..\\Resources\\Boss\\Effect\\Bullet\\Bullet\\Impact", Vector2::Zero, 0.05f);
		mAnimator->CreateAnimations(L"..\\Resources\\Boss\\Effect\\Bullet\\Bullet\\Loop", Vector2::Zero, 0.05f);
		mAnimator->CreateAnimations(L"..\\Resources\\Boss\\Effect\\Bullet\\Bullet\\Outro", Vector2::Zero, 0.016f);
		mAnimator->GetCompleteEvent(L"BulletImpact") = std::bind(&BossBullet::completeImpact, this);
		mAnimator->GetCompleteEvent(L"BulletOutro") = std::bind(&BossBullet::completeOutro, this);
		mAnimator->Play(L"BulletLoop", true);

		mCollider = AddComponent<Collider>();

		GameObject::Initialize();

		// 쓸 때까지 꺼 둔다
		SetState(eState::Pause);
	}
	void BossBullet::Update()
	{
		float dt = (float)Time::DeltaTime();
		mTime += dt;

		if (mBulletState == eBulletState::Fly)
		{
			// 약한 유도: 플레이어 쪽으로 가속하되 처음 속도보다 빨라지지 않는다
			if (mHoming > 0.0f)
			{
				PlayeScene* scene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
				Skul* skul = (scene != nullptr) ? scene->GetSkul() : nullptr;
				if (skul != nullptr)
				{
					Vector2 target = skul->GetComponent<Transform>()->GetPos();
					Vector2 toPlayer = target - mCenter;
					float distance = toPlayer.Length();
					if (distance > 1.0f)
						mVelocity += toPlayer * (mHoming * dt / distance);

					float speed = mVelocity.Length();
					if (speed > mSpeed && speed > 0.0f)
						mVelocity = mVelocity * (mSpeed / speed);
				}
			}

			mCenter += mVelocity * dt;

			if (mCenter.y >= BossBulletFloorY - 25.0f * mScale)
			{
				Pop();
			}
			else if (mCenter.x < -400.0f || mCenter.x > 3750.0f || mCenter.y < 200.0f || mTime > 8.0f)
			{
				// 화면 밖: 터지는 연출 없이 바로 거둔다
				SetState(eState::Pause);
				return;
			}
		}

		place();
		GameObject::Update();
	}
	void BossBullet::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void BossBullet::Release()
	{
		GameObject::Release();
	}

	void BossBullet::OnCollisionEnter(Collider* other)
	{
		tryHitPlayer(other);
	}
	void BossBullet::OnCollisionStay(Collider* other)
	{
		tryHitPlayer(other);
	}
	void BossBullet::OnCollisionExit(Collider* other)
	{
	}

	void BossBullet::Charge(Vector2 center, float scale)
	{
		SetState(eState::Active);
		mBulletState = eBulletState::Charge;
		mCenter = center;
		mVelocity = Vector2::Zero;
		mScale = scale;
		mHoming = 0.0f;
		mDamage = 0;
		mTime = 0.0f;

		GetComponent<Transform>()->SetScale(Vector2(scale, scale));
		float hitSize = BossBulletHitSize * scale;
		mCollider->SetSize(Vector2(hitSize, hitSize));

		mAnimator->Play(L"BulletImpact", false);
		place();
	}

	void BossBullet::Launch(Vector2 velocity, int damage, float homing)
	{
		mBulletState = eBulletState::Fly;
		mVelocity = velocity;
		mSpeed = velocity.Length();
		mDamage = damage;
		mHoming = homing;
		mTime = 0.0f;
	}

	void BossBullet::Pop()
	{
		if (mBulletState == eBulletState::Pop || GetState() != eState::Active)
			return;

		mBulletState = eBulletState::Pop;
		mVelocity = Vector2::Zero;
		mDamage = 0;
		mAnimator->Play(L"BulletOutro", false);
	}

	void BossBullet::place()
	{
		// 그림 위치: 확대한 그림의 가운데가 mCenter에 오게 (위 BossBulletCell 설명 참고)
		Vector2 shift = Vector2(BossBulletCell / 2.0f * (1.0f - mScale), BossBulletCell * (1.0f - mScale / 2.0f));
		GetComponent<Transform>()->SetPos(mCenter + shift);

		// 충돌 상자는 mCenter를 가운데로
		Vector2 size = mCollider->GetSize();
		mCollider->SetCenter(Vector2(-size.x / 2.0f - shift.x, -size.y / 2.0f - shift.y));
	}

	void BossBullet::tryHitPlayer(Collider* other)
	{
		if (mBulletState != eBulletState::Fly || mDamage <= 0)
			return;
		if (dynamic_cast<Skul*>(other->GetOwner()) == nullptr)
			return;

		// 원작: 대시(무적) 중에 닿은 투사체는 피해 없이 사라진다
		if (!PlayerStats::IsInvincible())
			PlayerStats::TakeDamage(mDamage, mCenter);
		Pop();
	}

	void BossBullet::completeImpact()
	{
		mAnimator->Play(L"BulletLoop", true);
	}

	void BossBullet::completeOutro()
	{
		SetState(eState::Pause);
	}
}
