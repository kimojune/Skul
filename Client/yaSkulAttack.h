#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;

	class SkulAttack : public GameObject
	{
	public:
		SkulAttack();
		~SkulAttack();

		virtual void Initialize();
		virtual void Update();
		virtual void Render(HDC hdc);
		virtual void Release();

		virtual void OnCollisionEnter(class Collider* other);
		virtual void OnCollisionStay(class Collider* other);
		virtual void OnCollisionExit(class Collider* other);

	private:
		Animator* mAnimator;
		eDirection mDirect;
	};

}
