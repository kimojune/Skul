#pragma once
#include "yaGameObject.h"


namespace ya
{
	class Animator;
	class Rigidbody;
	class Monster : public GameObject
	{
	public:
		Monster();
		~Monster();

		enum class eMonsterState
		{
			Idle,
			Move,
			Attack,
			LangeAttack,
			Hit,
			Dead,

		};
		
		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		eMonsterState GetMonsterState() { return mState; }
		void SetMonsterState(eMonsterState state) { mState = state; }

		//virtual void OnCollisionEnter(class Collider* other) override;
		//virtual void OnCollisionStay(class Collider* other) override;
		//virtual void OnCollisionExit(class Collider* other) override;


	protected:

		virtual void Idle();
		virtual void Move();
		virtual void Attack();
		virtual void LangeAttack();
		virtual void Hit();
		virtual void Dead();

		void CompleteIdle();
		void CompleteHit();
		void EndIdle();

	private:
		Animator* mAnimator;
		eMonsterState mState;
		Rigidbody* mRigidbody;
		eDirection mDirection;

		float mTime;
		bool mbPlay;

		int mHP;
	};
}
