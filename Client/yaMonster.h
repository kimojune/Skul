#pragma once
#include "yaGameObject.h"


namespace ya
{
	class Animator;
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

		virtual void Idle();
		virtual void Move();
		virtual void Attack();
		virtual void LangeAttack();
		virtual void Hit();
		virtual void Dead();


		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

	private:
		Animator* mAnimator;
		eMonsterState mState;
	};
}
