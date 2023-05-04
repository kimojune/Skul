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


	SkulHead* GetSkulHead() { return mSkulHead; }

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
