#pragma once
#include "yaPlayeScene.h"

namespace ya
{
class Stage1: public PlayeScene
{
public:
	Stage1();
	~Stage1();

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Render(HDC hdc) override;
	virtual void Release() override;

	virtual void OnEnter() override;
	virtual void OnExit() override;

private:
};

}
