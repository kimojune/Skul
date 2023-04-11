#pragma once
#include "yaGameObject.h"
#include "yaSkulHead.h"

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

		void SetHead(bool head) { mHead = head; }
		bool GetHead() { return mHead ;}

		eDirection GetDirect() { return mDirect;}

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
		void EndFall();

		virtual void StartSkillA();
		virtual void CompleteSkillA();
		virtual void EndSkillA();

		virtual void StartSkillS();
		virtual void CompleteSkillS();
		virtual void EndSkillS();

	protected:
		void	Idle();
		void	Move();
		void	Dash();
		void	Jump();
		void	Fall();
		void	Attack();

		virtual void	SkillA();
		virtual void	SkillS();
		void	Jumpattack();

		eDirection mDirect;
		eSkulState mState;

	private:
		Animator* mAnimator;
		Rigidbody* mRigidbody;

		bool mHead;
		bool SecondAttack;

		UINT AttackCount;
		UINT MoveCount;

		SkulHead* mSkulHead;

	};

}
