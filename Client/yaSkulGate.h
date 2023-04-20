#pragma once
#include "yaGate.h"

namespace ya
{

	class SkulGate : public Gate
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
