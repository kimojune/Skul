#include "yaTitleBG.h"
#include "yaResources.h"
#include "yaRenderer.h"



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
		Renderer::DrawImage(mImage, 0, 0, 1600, 900, 0, 0, mImage->GetWidth(), mImage->GetHeight());
	}
	void TitleBG::Release()
	{
	}
}