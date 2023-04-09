#pragma once
#include "yaSkul.h"

namespace ya
{

class BasicSkul : public Skul
{
public:
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Render(HDC hdc) override;
	virtual void Release() override;

private:

};

}
