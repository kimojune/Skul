#pragma once
#include "yaResource.h"

namespace ya
{
	class Image : public Resource
	{
	public:
		static Image* Create(const std::wstring& name, UINT widht, UINT height, COLORREF rgb = RGB(255,255,255));
		
		Image();
		~Image();

		// HDC/HBITMAP 을 소유하므로 복사하면 두 번 해제된다.
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

		virtual HRESULT Load(const std::wstring& path) override;

		HDC GetHdc() { return mHdc; }
		HBITMAP GetBitmap() { return mBitmap; }
		UINT GetWidth() { return mWidth; }
		UINT GetHeight() { return mHeight; }

		COLORREF GetPixel(int x, int y);
		void SetPixel(int x, int y, COLORREF color);

	private:
		HBITMAP mBitmap;
		HBITMAP mOldBitmap; // mHdc 를 만들 때 원래 선택돼 있던 비트맵. 해제 전에 되돌린다
		HDC mHdc;
		UINT mWidth;
		UINT mHeight;
	};
}
