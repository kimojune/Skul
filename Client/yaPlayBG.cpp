#include "yaPlayBG.h"
#include "yaResources.h"
#include "yaApplication.h"

extern ya::Application application;

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
		TransparentBlt(hdc, 0, 0, application.GetWidth(), application.GetHeight(), mImage->GetHdc(), 0, 0, mImage->GetWidth(), mImage->GetHeight(), RGB(255, 0, 255));
	}
	void PlayBG::Release()
	{
	}
}
