#pragma once
#include "yaSkul.h"


namespace ya
{
	extern  Skul* mSkuls[(UINT)Skul::eSkulType::End];

	class Nohead : public Skul
	{
	public:
		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;
		
		
		virtual void SkillA() override;
		virtual void SkillS() override;

	private:
		Animator* mAnimator;
	};

}
