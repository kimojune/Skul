#include "yaBossBG.h"
#include "yaResources.h"
#include "yaCamera.h"
#include "yaTransform.h"

namespace ya
{
	BossBG::BossBG()
	{
	}
	BossBG::~BossBG()
	{
	}
	void BossBG::Initialize()
	{
		mImage = Resources::Load<Image>(L"BossBG", L"..\\Resources\\BG\\Boss\\Boss_BG_2_1.bmp");
		//mImage[0] = Resources::Load<Image>(L"PlayLayer1", L"..\\Resources\\BG\\Stage1\\stage1x2.bmp");
	}
	void BossBG::Update()
	{
	}
	void BossBG::Render(HDC hdc)
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 renderPos = Camera::CaluatePos(tr->GetPos());

		
		TransparentBlt(hdc
			, renderPos.x, renderPos.y + 200
			, mImage->GetWidth()*2+ 200, mImage->GetHeight()*2
			, mImage->GetHdc()
			, 0, 0
			, mImage->GetWidth(), mImage->GetHeight()
			, RGB(255, 0, 255));
	}
	void BossBG::Release()
	{
	}
}