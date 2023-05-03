#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class Boss_Body : public GameObject
	{
	public:
		Boss_Body();
		~Boss_Body();

		enum class eBodyState
		{
			Idle,
			UP,
			Down,
			Left,
			Right,
		};

		virtual void Initialize()override;
		virtual void Update()override;
		virtual void Render(HDC hdc)override;
		virtual void Release()override;

		void SetBodyState(eBodyState state) { mBodyState = state; }
		void SetPlayed(bool play) { mPlayed = false; }

	protected:
		void Idle();
		void UP();
		void Down();
		void Left();
		void Right();

	private:
		Animator* mAnimator;
		Image* mImage[3];
		eBodyState mBodyState;

		Vector2 mPrevPos;
		Vector2 mLeftPos;
		Vector2 mRightPos;
		Vector2 mUpPos;
		Vector2 mDownPos;
		
		float mTime;
		bool mPlayed;
	};

}