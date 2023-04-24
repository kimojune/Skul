#include "yaTitleLogo.h"
#include "yaResources.h"
#include "yaTime.h"
#include "yaTransform.h"

namespace ya
{
	TitleLogo::TitleLogo()
		:mTime(0)
	{
	}

	TitleLogo::~TitleLogo()
	{
	}
	void TitleLogo::Initialize()
	{
		mImage = Resources::Load<Image>(L"PlayBG", L"..\\Resources\\BG\\Title\\Logo_Alpha.bmp");
		GameObject::Initialize();

	}
	void TitleLogo::Update()
	{
		mTime += Time::DeltaTime();
		if (mTime >= 10)
		{
			mTime = 0;
		}
		GameObject::Update();

	}
	void TitleLogo::Render(HDC hdc)
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();


		BLENDFUNCTION func = {};
		func.BlendOp = AC_SRC_OVER;
		func.BlendFlags = 0;
		func.AlphaFormat = AC_SRC_ALPHA;
		func.SourceConstantAlpha = 100 * mTime; // 0(투명) ~ 255(불투명) 알파값

		AlphaBlend(hdc,pos.x , pos.y, mImage->GetWidth(), mImage->GetHeight(), mImage->GetHdc(), 0, 0, mImage->GetWidth(), mImage->GetHeight(), func);

		GameObject::Render(hdc);

	}
	void TitleLogo::Release()
	{
	}
}