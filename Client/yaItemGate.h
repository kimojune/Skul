#pragma once
#include "yaGate.h"

namespace ya
{

	class ItemGate : public Gate
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
