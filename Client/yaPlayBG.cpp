#include "yaPlayBG.h"
#include "yaResources.h"
#include "yaCamera.h"
#include "yaTransform.h"

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
		mImage[0] = Resources::Load<Image>(L"PlayLayer1", L"..\\Resources\\BG\\Stage1\\stage1x2.bmp");
		//mImage[0] = Resources::Load<Image>(L"PlayLayer1", L"..\\Resources\\BG\\Stage1\\stage1x2.bmp");
		


	}
	void PlayBG::Update()
	{
	}
	void PlayBG::Render(HDC hdc)
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 renderPos = Camera::CaluatePos(tr->GetPos());
		
		Count = renderPos.x / -1600;
		
		TransparentBlt(hdc
			, Count * 1600 + renderPos.x, renderPos.y
			, mImage[0]->GetWidth(), mImage[0]->GetHeight()
			, mImage[0]->GetHdc()
			, 0, 0
			, mImage[0]->GetWidth(), mImage[0]->GetHeight()
			, RGB(255, 0, 255));

		//TransparentBlt(hdc, 0, 900 - mImage->GetHeight(), mImage->GetWidth(), 900, mImage->GetHdc(), 0, 0, mImage->GetWidth(), mImage->GetHeight(), RGB(255, 0, 255));
	}
	void PlayBG::Release()
	{
	}
}
