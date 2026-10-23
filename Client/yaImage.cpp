#include "yaImage.h"
#include "yaApplication.h"
#include "yaResources.h"

extern ya::Application application;

namespace ya
{
	Image* Image::Create(const std::wstring& name, UINT widht, UINT height, COLORREF rgb)
	{
		if (widht == 0 || height == 0)
			return nullptr;

		Image* image = Resources::Find<Image>(name);
		if (image != nullptr)
			return image;

		image = new Image();
		HDC mainHdc = application.GetHdc();

		image->mBitmap = CreateCompatibleBitmap(mainHdc, widht, height);
		
		image->mHdc = CreateCompatibleDC(mainHdc);

		image->mOldBitmap = (HBITMAP)SelectObject(image->mHdc, image->mBitmap);

		image->mWidth = widht;
		image->mHeight = height;

		image->SetKey(name);
		Resources::Insert<Image>(name, image);

		HBRUSH brush = CreateSolidBrush(rgb);
		HBRUSH oldbrush = (HBRUSH)SelectObject(image->GetHdc(), brush);
		Rectangle(image->GetHdc(), -1, -1, image->mWidth + 1, image->mHeight + 1);

		SelectObject(image->GetHdc(), oldbrush);
		DeleteObject(brush);

		return image;
	}
	Image::Image()
		: mBitmap(NULL)
		, mOldBitmap(NULL)
		, mHdc(NULL)
		, mWidth(0)
		, mHeight(0)
	{


	}

	Image::~Image()
	{
		// 원래 비트맵을 DC에 되돌려야 우리 비트맵이 선택 해제되어 지울 수 있다.
		if (mHdc != NULL)
		{
			if (mOldBitmap != NULL)
				SelectObject(mHdc, mOldBitmap);

			DeleteDC(mHdc);
			mHdc = NULL;
		}

		if (mBitmap != NULL)
		{
			DeleteObject(mBitmap);
			mBitmap = NULL;
		}
	}

	HRESULT Image::Load(const std::wstring& path)
	{
		mBitmap = (HBITMAP)LoadImageW(nullptr
			, path.c_str(), IMAGE_BITMAP
			, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

		if (mBitmap == nullptr)
			return E_FAIL;

		BITMAP bitInfo = {};
		GetObject(mBitmap, sizeof(BITMAP), &bitInfo);

		mWidth = bitInfo.bmWidth;
		mHeight = bitInfo.bmHeight;

		HDC mainDC = application.GetHdc();
		mHdc = CreateCompatibleDC(mainDC);

		mOldBitmap = (HBITMAP)SelectObject(mHdc, mBitmap);

		return S_OK;
	}

	COLORREF Image::GetPixel(int x, int y)
	{
		return ::GetPixel(mHdc, x, y);
	}
	void Image::SetPixel(int x, int y, COLORREF color)
	{
		::SetPixel(mHdc, x, y, color);
	}
}
