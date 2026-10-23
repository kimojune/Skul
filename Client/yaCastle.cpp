#include "yaCastle.h"
#include "yaResources.h"
#include "yaCamera.h"
#include "yaTransform.h"
#include "yaImage.h"
#include "yaRenderer.h"

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


		Renderer::DrawImage(mImage
			, (int)(renderPos.x - 50), (int)renderPos.y
			, 700, 1800
			, 0, 0
			, mImage->GetWidth(), mImage->GetHeight());
	}
}