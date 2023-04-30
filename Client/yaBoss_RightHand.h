#pragma once


#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class Image;
	class Chapter1_Boss;
	class Boss_RightHand : public GameObject
	{
	public:
		Boss_RightHand();
		~Boss_RightHand();

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

		void SetHandState(eHandState state) { mHandState = state; }
		void SetTargetPos(Vector2 pos) { mTargetPos = pos; }
		void SetHandDirection(eDirection direct) { mDirection = direct; }
		Animator* GetAnimator() { return mAnimator; }


	protected:

		void Idle();
		void Down();
		void Punch();
		void Smash();

		void StartDown();
		//void CompleteDown() ;
		//void EndDown() ;

		void StartPunch();
		void CompletePunch();
		void EndPunch();

		void StartSmash();
		void CompleteSmash();
		void EndSmash();

	private:
		Animator* mAnimator;
		Image* mLeftImage[4];
		Image* mRightImage[4];
		eDirection mDirection;
		eHandState mHandState;
		Vector2 mTargetPos;
		Vector2 mPrevPos;
		Chapter1_Boss* mBoss;

	};

}