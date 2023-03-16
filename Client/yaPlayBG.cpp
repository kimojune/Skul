#include "yaPlayBG.h"
#include "yaResources.h"


namespace ya
{
	PlayBG::PlayBG()
	{
	}
	PlayBG::~PlayBG()
	{
	}
	void PlayBG::Initialize()
	{
		mImage = Resources::Load<Image>(L"PlayBG", L"..\\Resources\\BG\\Stage1\\Stage1.bmp");
	}
	void PlayBG::Update()
	{
	}
	void PlayBG::Render(HDC hdc)
	{
		TransparentBlt(hdc, 0, 0, 1600, 900, mImage->GetHdc(), 0, 0, mImage->GetWidth(), mImage->GetHeight(), RGB(255, 0, 255));
	}
	void PlayBG::Release()
	{
	}
}
