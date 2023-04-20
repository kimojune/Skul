#pragma once
#include "yaGate.h"

namespace ya
{

class BossGate : public Gate
{
public:
	BossGate();
	~BossGate();
	virtual void Initialize();
	virtual void Update();
	virtual void Render(HDC hdc);
	virtual void Release();


private:
	Animator* mAnimator;

};

}
