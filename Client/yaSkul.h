#pragma once
#include "yaGameObject.h"
#include "yaSkulHead.h"

namespace ya
{
	class Animator;
	class Rigidbody;
	class SkulAttack;
	class Skul : public GameObject
	{
	public:
		enum class eSkulType
		{
			Basic,
			Nohead,
			End,
		};

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
			ChangeSkill,
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


		Skul* SwitchSkul(eSkulType type);
		void SetDirect(eDirection direct);
		bool CheckAnimation(const std::wstring& name);
		eDirection GetDirect() { return mDirect;}
	
		void SetSkulState(eSkulState state) { mState = state; }
		eSkulState GetSkulState() {return mState; }

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

		virtual void StartChangeSkill();
		virtual void CompleteChangeSkill();
		virtual void EndChangeSkill();

		
		

	protected:
		void	Idle();
		void	Move();
		void	Dash();
		void	Jump();
		void	Fall();
		void	Attack();

		virtual void	SkillA();
		virtual void	SkillS();
		virtual void	ChangeSkill();
		void	Jumpattack();

		eDirection mDirect;
		eSkulState mState;

	private:
		Animator* mAnimator;
		Rigidbody* mRigidbody;
		SkulAttack* mAttack;

		bool SecondAttack;
		bool SecondDash;
		bool SecondJump;

		//Skul* mSecondSkul;

		UINT AttackCount;

		

	};

}
