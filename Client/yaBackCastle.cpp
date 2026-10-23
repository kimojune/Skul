#include "yaBackCastle.h"
#include "yaResources.h"
#include "yaCamera.h"
#include "yaTransform.h"
#include "yaImage.h"
#include "yaRenderer.h"

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

		Renderer::DrawImage(mImage
			, (int)renderPos.x, (int)renderPos.y
			, 2112, 1800
			, 0, 0
			, mImage->GetWidth(), mImage->GetHeight());



	}
}