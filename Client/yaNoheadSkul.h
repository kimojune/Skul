#pragma once
#include "yaSkul.h"


namespace ya
{
	extern  Skul* mSkuls[(UINT)Skul::eSkulType::End];

	class Sound;

	class Nohead : public Skul
	{
	public:
		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;


		virtual void SkillA() override;
		virtual void SkillS() override;

		// 머리 없이는 쓰러지는 애니메이션이 없어서 머리를 되찾은 Basic이 쓰러진다
		virtual void Die() override;

		// 머리를 되찾아 Basic으로 교대한다 (그 자리에서). 줍기·쿨다운 복귀·안전 복귀·씬 이동이 쓴다.
		// resetCooldown: 머리를 "회수"한 경우 A 쿨다운 초기화 (원작: 머리를 회수하면 쿨다운 초기화)
		Skul* TakeHeadBack(bool resetCooldown, bool playSound);
		// 지금 교대해도 되는가 (대시·순간이동·사망 중에는 끝난 뒤에)
		bool CanTakeHeadBack();

	protected:
		virtual bool CanUseSkillS() override;

	private:
		Animator* mAnimator;
		Sound* Skul_SkullBack;
	};

}
