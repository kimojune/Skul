#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class Collider;

	// 보스 에너지볼. Chapter1_Boss가 미리 만들어 두고(Pause) 돌려 쓴다.
	// Charge로 나타나서 제자리에 모이고, Launch로 날아가며, 플레이어·바닥에 닿거나 화면 밖으로 나가면 사라진다.
	// 위치는 구체의 가운데다. scale을 키우면 큰 구체(조준 구체, 2페이즈 비)가 된다.
	class BossBullet : public GameObject
	{
	public:
		BossBullet();
		~BossBullet();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		virtual void OnCollisionEnter(class Collider* other) override;
		virtual void OnCollisionStay(class Collider* other) override;
		virtual void OnCollisionExit(class Collider* other) override;

		// center에 나타나 모이기 시작한다 (피해 없음)
		void Charge(Vector2 center, float scale);
		// velocity(px/초)로 날아간다. homing은 플레이어 쪽으로 꺾는 가속도(px/초²), 0이면 직선
		void Launch(Vector2 velocity, int damage, float homing = 0.0f);
		// 사라지는 애니메이션을 재생하고 멈춘다
		void Pop();

		bool IsFree() { return GetState() != eState::Active; }
		bool IsCharging() { return GetState() == eState::Active && mBulletState == eBulletState::Charge; }
		bool IsFlying() { return GetState() == eState::Active && mBulletState == eBulletState::Fly; }
		Vector2 GetCenter() { return mCenter; }

	private:
		enum class eBulletState
		{
			Charge,
			Fly,
			Pop,
		};

		void place();
		void tryHitPlayer(Collider* other);
		void completeImpact();
		void completeOutro();

	private:
		Animator* mAnimator;
		Collider* mCollider;
		eBulletState mBulletState;
		Vector2 mCenter;
		Vector2 mVelocity;
		float mSpeed;
		float mScale;
		float mHoming;
		float mTime;
		int mDamage;
	};
}
