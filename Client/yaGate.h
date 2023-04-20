#pragma once
#include "yaStruct.h"

namespace ya
{
	class Animator;
class Gate : public Struct
{

public:
	Gate();
	virtual ~Gate();

	virtual void Close();
	virtual void Open();

	virtual void Initialize();
	virtual void Update();
	virtual void Render(HDC hdc);
	virtual void Release();

	virtual void OnCollisionEnter(class Collider* other);
	virtual void OnCollisionStay(class Collider* other);
	virtual void OnCollisionExit(class Collider* other);

private:
	Animator* mAnimator;
	bool mOpen;
};

}
