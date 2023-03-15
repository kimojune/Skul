#pragma once
#include "yaGameObject.h"
#include "yaImage.h"

namespace ya
{
	class Animator;
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
			FallRepeat,
			AttackA,
			AttackB,
			JumpAttack,
			Skill,
			Fall_Dead,

		};

		enum class eSkulDirection
		{
			Left,
			Right,
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

	private:
		void	Idle();
		void	Move();
		void	Dash();
		void	Jump();
		void	Fall();
		void	Fallrepeat();
		void	AttackA();
		void	AttackB();
		void	Jumpattack();
		void	Skill();
		void idleCompleteEvent();

	private:
		eSkulState mState;
		eSkulDirection mDirect;
		Animator* mAnimator;
		
	};
}
