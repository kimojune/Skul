#include "yaTitleLogo.h"
#include "yaResources.h"

namespace ya
{
	TitleLogo::TitleLogo()
	{
	}

	TitleLogo::~TitleLogo()
	{
	}
	void TitleLogo::Initialize()
	{
		//mImage = Resources::Load<Image>(L"PlayBG", L"..\\Resources\\BG\\Title\\Logo.bmp");

	}
	void TitleLogo::Update()
	{
	}
	void TitleLogo::Render(HDC hdc)
	{

		BLENDFUNCTION func = {};
		func.BlendOp = AC_SRC_OVER;
		func.BlendFlags = 0;
		func.AlphaFormat = AC_SRC_ALPHA;
		func.SourceConstantAlpha = 255; // 0(투명) ~ 255(불투명) 알파값

		//AlphaBlend(hdc, 0, 0, mImage->GetWidth(), mImage->GetHeight(), mImage->GetHdc(), 0, 0, mImage->GetWidth(), mImage->GetHeight(), func);
	}
	void TitleLogo::Release()
	{
	}
}