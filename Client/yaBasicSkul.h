#pragma once
#include "yaSkul.h"

namespace ya
{

	class Sound;
class Basic : public Skul
{
public:
	Basic();
	~Basic();

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Render(HDC hdc) override;
	virtual void Release() override;

	virtual void SkillA() override;
	virtual void SkillS() override;
	virtual void ChangeSkill() override;

	virtual void StartSkillA() override;
	virtual void CompleteSkillA() override;
	virtual void EndSkillA() override;

	virtual void StartSkillS() override;
	virtual void CompleteSkillS() override;
	virtual void EndSkillS() override;

	virtual void StartChangeSkill() override;
	virtual void CompleteChangeSkill() override;
	virtual void EndChangeSkill() override;

	virtual void Die() override;


	SkulHead* GetSkulHead() { return mSkulHead; }

protected:
	// 머리를 던질 수 있는가 (쿨다운, 공중 1회 제한은 PlayerStats::CanThrowHead)
	virtual bool CanUseSkillA() override;

private:
	Animator* mAnimator;
	SkulHead* mSkulHead;
	Sound* Skul_SkullThrowing;
	Sound* Skul_Reborn_Rise;
	bool mbPlay;
	float mTime;
	bool mComplete;
};

}
