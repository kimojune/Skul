#include "yaCastle.h"
#include "yaResources.h"
#include "yaCamera.h"
#include "yaTransform.h"
#include "yaImage.h"

namespace ya
{
	Castle::Castle()
	{
	}
	Castle::~Castle()
	{
	}
	void Castle::Initialize()
	{
		mImage = Resources::Load<Image>(L"Castle", L"..\\Resources\\BG\\Stage1\\Castle.bmp");
	}
	void Castle::Render(HDC hdc)
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 renderPos = Camera::CaluatePos(tr->GetPos());


		TransparentBlt(hdc
			, renderPos.x - 50, renderPos.y
			, 700, 1800
			, mImage->GetHdc()
			, 0, 0
			, mImage->GetWidth(), mImage->GetHeight()
			, RGB(255, 0, 255));
	}
}