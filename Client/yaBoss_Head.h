#pragma once

#include "yaGameObject.h"

namespace ya
{
	class Animator;
class Boss_Head : public GameObject
{
public:
	Boss_Head();
	~Boss_Head();

	enum class eHeadState
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

	void SetPlayed(bool play) { mPlayed = play; }
	void SetHeadState(eHeadState state) { mHeadState = state; }
	int GetHeadHP() { return mHP; }
	void Hit();

protected:
	void Idle();
	void UP();
	void Down();
	void Left();
	void Right();

private:
	Animator* mAnimator;
	Image* mImage[3];
	float mTime;
	
	Vector2 mPrevPos;
	Vector2 mLeftPos;
	Vector2 mRightPos;
	Vector2 mUpPos;
	Vector2 mDownPos; 

	eHeadState mHeadState;

	int mHP;
	bool mPlayed;
};

}