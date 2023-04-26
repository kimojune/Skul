#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Boss_Body;
	class Boss_Head;
	class Boss_Hand;
	class BossBullet;

class Chapter1_Boss: public GameObject
{
public:
	Chapter1_Boss();
	~Chapter1_Boss();
	
	virtual void Initialize()override;
	virtual void Update()override;
	virtual void Render(HDC hdc)override;
	virtual void Release()override;

private:
	Boss_Body* mBody;
	Boss_Hand* mHand[2];
	Boss_Head* mHead;
	BossBullet mBullet[8];
};

}
