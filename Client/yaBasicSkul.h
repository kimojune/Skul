#pragma once
#include "yaSkul.h"

namespace ya
{


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

	virtual void StartSkillA() override;
	virtual void CompleteSkillA() override;
	virtual void EndSkillA() override;

	virtual void StartSkillS() override;
	virtual void CompleteSkillS() override;
	virtual void EndSkillS() override;

	SkulHead* GetSkulHead() { return mSkulHead; }

private:
	Animator* mAnimator;
	SkulHead* mSkulHead;

	bool mbPlay;
};

}
