#pragma once
#include "yaStruct.h"

namespace ya
{

class Castle : public Struct
{
public:
	Castle();
	~Castle();
	
	virtual void Initialize() override;
	virtual void Render(HDC hdc) override;


private:
	Image* mImage;
};

}
