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
		
	private:
		void	idle();
		void	move();
		void	dash();
		void	jump();
		void	fall();
		void	fallrepeat();
		void	attackA();
		void	attackB();
		void	jumpattack();
		void	skill();
		void idleCompleteEvent();

	private:
		eSkulState mState;
		eSkulDirection mDirect;
		Animator* mAnimator;
		
	};
}
