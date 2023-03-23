#pragma once
#include "yaPlayer.h"


namespace ya
{
	class Animator;
	class Rigidbody;
	class Skul : public Player
	{
	public:

		enum class eSkulState
		{
			Idle,
			Move,
			Dash,
			Jump,
			Fall,
			Attack,
			Attack2,
			JumpAttack,
			SkillA,
			SkillS,
			Fall_Dead,
			End,
		};


		Skul();
		~Skul();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;
		
		virtual void OnCollisionEnter(class Collider* other) override;
		virtual void OnCollisionStay(class Collider* other) override;
		virtual void OnCollisionExit(class Collider* other) override;
		
		virtual void SetHead(bool head) { mHead = head; }

		void StartDash();
		void CompleteDash();
		void EndDash();

		void StartAttack();
		void CompleteAttack();
		void EndAttack();

		void StartJumpAttack();
		void CompleteJumpAttack();
		void EndJumpAttack();

		void StartJump();
		void CompleteJump();
		void EndJump();

		void StartFall();
		void CompleteFall();
		void EndFall ();

		void StartShoot();
		void CompleteShoot();
		void EndShoot();
		
	protected:
		void	Idle();
		void	Move();
		void	Dash();
		void	Jump();
		void	Fall();
		void	Attack();

		void	SkillA();
		void	SkillB();
		void	Jumpattack();

	private:
		eSkulState mState;
		eDirection mDirect;
		Animator* mAnimator;
		Rigidbody* mRigidbody;
		bool mHead;
		int AttackCount;
		bool SecondAttack;
		};
}
