#include "yaTitleLogo.h"
#include "yaResources.h"
#include "yaTime.h"
#include "yaTransform.h"
#include "yaRenderer.h"

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


		// 이미지의 알파 채널로 섞고, 전체 알파는 시간에 따라 바뀐다
		BYTE alpha = (BYTE)(100 * mTime); // 0(투명) ~ 255(불투명) 알파값

		Renderer::DrawImage(mImage, (int)pos.x, (int)pos.y, mImage->GetWidth(), mImage->GetHeight()
			, 0, 0, mImage->GetWidth(), mImage->GetHeight(), eImageBlend::AlphaChannel, alpha);

		GameObject::Render(hdc);

	}
	void TitleLogo::Release()
	{
	}
}