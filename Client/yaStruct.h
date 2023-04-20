#pragma once
#include "yaGameObject.h"


namespace ya
{
	class Image;
class Struct : public GameObject
{
public:

	Struct();
	virtual ~Struct();

	virtual void Initialize();
	virtual void Update();
	virtual void Render(HDC hdc);
	virtual void Release();

private:
	Image* mImage;
	
};


}