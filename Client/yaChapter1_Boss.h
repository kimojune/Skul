#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Boss_Body;
	class Boss_Head;
	class Boss_Hand;
	class Boss_RightHand;
	class Boss_Chin;
	class BossBullet;

class Chapter1_Boss: public GameObject
{
public:
	Chapter1_Boss();
	~Chapter1_Boss();
	
	enum class eBossState
	{
		Idle,
		Punch,
		Smash,
		RangeAttack,
		Dead,
		End,
	};

	virtual void Initialize()override;
	virtual void Update()override;
	virtual void Render(HDC hdc)override;
	virtual void Release()override;

	eBossState GetBossState() { return mState; }

private:

	void Idle();
	void Punch();
	void Smash();
	void RangeAttack();
	void Dead();

private:
	Boss_Body* mBody;
	Boss_Hand* mLeftHand;
	Boss_RightHand* mRightHand;
	Boss_Head* mHead;
	Boss_Chin* mChin;
	BossBullet* mBullet[8];
	eBossState mState;
	eBossState mprevState;


	Vector2 mLeftPos;
	Vector2 mRightPos;

	Transform* mbodytr;
	Transform* mheadtr;
	Transform* mchintr;
	float mTime;
};

}
