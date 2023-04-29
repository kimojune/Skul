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
		void SetTargetPos(Vector2 pos) { mTargetPos = pos; }
		Animator* GetAnimator() { return mAnimator; }
		



	protected:

		void Idle();
		void Down();
		void Punch();
		void Smash();

		void StartDown() ;
		//void CompleteDown() ;
		//void EndDown() ;
		
		void StartPunch() ;
		void CompletePunch() ;
		void EndPunch() ;

		void StartSmash() ;
		void CompleteSmash() ;
		void EndSmash() ;

	private:
		Animator* mAnimator;
		Image* mLeftImage[4];
		Image* mRightImage[4];
		eDirection mDirection;
		eHandState mHandState;
		Vector2 mTargetPos;
		Vector2 mPrevPos;
	};

}