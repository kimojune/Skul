#include "yaFadeInOut.h"
#include "yaResources.h"
#include "yaTime.h"
namespace ya
{


	FadeInOut::FadeInOut()
		:mTime(0.0f)
		, mImage(nullptr)
	{
	}
	FadeInOut::~FadeInOut()
	{
	}
	void FadeInOut::Initialize()
	{
		mImage = Resources::Load<Image>(L"Logo", L"..\\Resources\\BG\\Title\\1.bmp");
	}
	void FadeInOut::Update()
	{
	}
	void FadeInOut::Render(HDC hdc)
	{
		mTime += 255 * Time::DeltaTime() * 0.7;

		BLENDFUNCTION func = {};
		func.BlendOp = AC_SRC_OVER;
		func.BlendFlags = 0;
		func.AlphaFormat = AC_SRC_ALPHA;
		func.SourceConstantAlpha = mTime;

		AlphaBlend(hdc, -1, -1, 1602, 902, mImage->GetHdc(), 0, 0, mImage->GetWidth(), mImage->GetHeight(), func);

		if (mTime > 255)
		{
			mTime = 0;
		}
	}
	void FadeInOut::Release()
	{
	}
}