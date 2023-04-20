#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;

	class ItemGate : public GameObject
	{
	public:
		ItemGate();
		~ItemGate();
		virtual void Initialize();
		virtual void Update();
		virtual void Render(HDC hdc);
		virtual void Release();

	private:
		Animator* mAnimator;

	};

}
