#pragma once

#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class Image;
	class Boss_Hand : public GameObject
	{
	public:
		Boss_Hand();
		Boss_Hand(bool isRight);
		~Boss_Hand();

		enum class eHandState
		{
			Idle,
			Down,
			Punch,
			Smash,
		};

		virtual void Initialize()override;
		virtual void Update()override;
		virtual void Render(HDC hdc)override;
		virtual void Release()override;

		void SetState(eHandState state) { mHandState = state; }

	protected:

		void Idle();
		void Down();
		void Punch();
		void Smash();

	private:
		Animator* mAnimator;
		Image* mImage[3];
		eDirection mDirection;
		eHandState mHandState;
		Vector2 mTargetPos;
	};

}