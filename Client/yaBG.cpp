#include "yaBG.h"
#include "yaResources.h"
#include "yaSceneManager.h"

namespace ya
{
	BG::BG(eSceneType type)
	{
		mScene = type;
		mImage.resize((UINT)eSceneType::End);
	}
	BG::~BG()
	{
	}
	void BG::Initialize()
	{
		
		mImage[(UINT)eSceneType::Title] = Resources::Load<Image>(L"TitleBG", L"..\\Resources\\BG\\Title\\TitleBG.bmp");
		mImage[(UINT)eSceneType::Play] = Resources::Load<Image>(L"PlayBG", L"..\\Resources\\BG\\Stage1\\Stage1.bmp");
	

		ActiveBG = mImage[(UINT)eSceneType::Title];

	}
	void BG::Update()
	{
		ActiveBG = mImage[(UINT)mScene];
	}
	void BG::Render(HDC hdc)
	{

	TransparentBlt(hdc, 0, 0, 1600, 900, ActiveBG->GetHdc(), 0, 0, ActiveBG->GetWidth(), ActiveBG->GetHeight(), RGB(255, 0, 255));

	}

	void BG::Release()
	{
	}

	void BG::SetScene(eSceneType scene)
	{
		mScene = scene; 
	}
}