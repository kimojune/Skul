#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;

	class SkulGate : public GameObject
	{
	public:
		SkulGate();
		~SkulGate();
		virtual void Initialize() override;
		virtual void Update();
		virtual void Render(HDC hdc);
		virtual void Release();

	private:
		Animator* mAnimator;

	};

}
