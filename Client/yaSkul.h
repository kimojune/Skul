#pragma once
#include "yaGameObject.h"


namespace ya
{
	class Animator;
	class Rigidbody;
	class Skul : public GameObject
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

		void StartDash();
		void CompleteDash();
		void EndDash();

		void StartAttack();
		void CompleteAttack();
		void EndAttack();

		void StartJump();
		void CompleteJump();
		void EndJump();

		void StartFall();
		void CompleteFall();
		void EndFall ();

		void StartShoot();
		void CompleteShoot();
		void EndShoot();
		
	private:
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
		bool head;
		int AttackCount;
		bool SecondAttack;
		};
}
