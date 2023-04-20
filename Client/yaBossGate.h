#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class BossGate : public GameObject
	{
	public:
		BossGate();
		~BossGate();
		virtual void Initialize();
		virtual void Update();
		virtual void Render(HDC hdc);
		virtual void Release();

		virtual void OnCollisionEnter(class Collider* other) override;
		virtual void OnCollisionStay(class Collider* other)override;
		virtual void OnCollisionExit(class Collider* other)override;
	private:
		Animator* mAnimator;
		bool mbActivate;
		
	};

}
