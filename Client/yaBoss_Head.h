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

	virtual void Initialize()override;
	virtual void Update()override;
	virtual void Render(HDC hdc)override;
	virtual void Release()override;

private:
	Animator* mAnimator;
	Image* mImage[3];
};

}