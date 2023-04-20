#include "yaBackCastle.h"
#include "yaResources.h"
#include "yaCamera.h"
#include "yaTransform.h"
#include "yaImage.h"

namespace ya
{
	BackCastle::BackCastle()
	{
	}
	BackCastle::~BackCastle()
	{
	}
	void BackCastle::Initialize()
	{
		mImage = Resources::Load<Image>(L"BackCastle", L"..\\Resources\\BG\\Stage1\\BGcastle.bmp");

	}
	void BackCastle::Render(HDC hdc)
	{

		Transform* tr = GetComponent<Transform>();
		Vector2 renderPos = Camera::CaluatePos(tr->GetPos());

		TransparentBlt(hdc
			, renderPos.x, renderPos.y
			, 2112, 1800
			, mImage->GetHdc()
			, 0, 0
			, mImage->GetWidth(), mImage->GetHeight()
			, RGB(255, 0, 255));



	}
}