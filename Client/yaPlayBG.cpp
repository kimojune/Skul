#include "yaPlayBG.h"
#include "yaResources.h"
#include "yaCamera.h"
#include "yaTransform.h"
#include "yaRenderer.h"

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
		mImage[0] = Resources::Load<Image>(L"PlayLayer1", L"..\\Resources\\BG\\Stage1\\Stage1x2.bmp");
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
		
		Renderer::DrawImage(mImage[0]
			, (int)(Count * 1600 + renderPos.x), (int)renderPos.y
			, mImage[0]->GetWidth(), mImage[0]->GetHeight()
			, 0, 0
			, mImage[0]->GetWidth(), mImage[0]->GetHeight());
	}
	void PlayBG::Release()
	{
	}
}
