#include "yaTitleBG.h"
#include "yaResources.h"



namespace ya
{
	TitleBG::TitleBG()
	{
	}
	TitleBG::~TitleBG()
	{
	}
	void TitleBG::Initialize()
	{
		mImage = Resources::Load<Image>(L"TitleBG", L"..\\Resources\\BG\\Title\\TitleBG.bmp");

	}
	void TitleBG::Update()
	{

	}
	void TitleBG::Render(HDC hdc)
	{
		TransparentBlt(hdc, 0, 0, 1600, 900, mImage->GetHdc(), 0, 0, mImage->GetWidth(), mImage->GetHeight(), RGB(255, 0, 255));
	}
	void TitleBG::Release()
	{
	}
}