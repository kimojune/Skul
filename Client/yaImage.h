#pragma once
#include "yaResource.h"

namespace ya
{
	namespace graphics
	{
		class Texture;
	}

	// 픽셀은 GDI 비트맵(CPU)에 있다. GetPixel/SetPixel 과 GDI 그리기는 이 비트맵을 쓰고,
	// Direct3D 11 로 그릴 때는 처음 그리는 순간 GPU 텍스처로 올린다 (GetTexture).
	class Image : public Resource
	{
	public:
		static Image* Create(const std::wstring& name, UINT widht, UINT height, COLORREF rgb = RGB(255,255,255));

		Image();
		~Image();

		// HDC/HBITMAP 을 소유하므로 복사하면 두 번 해제된다.
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

		// .bmp 는 LoadImageW, .png 는 WIC 로 읽는다 (PNG 는 알파 채널을 premultiplied 로 가진다)
		virtual HRESULT Load(const std::wstring& path) override;

		HDC GetHdc() { return mHdc; }
		HBITMAP GetBitmap() { return mBitmap; }
		UINT GetWidth() { return mWidth; }
		UINT GetHeight() { return mHeight; }

		COLORREF GetPixel(int x, int y);
		void SetPixel(int x, int y, COLORREF color);

		// 모든 픽셀을 32비트 BGRA 로 읽는다 (위 줄부터, 한 줄 GetWidth() * 4 바이트)
		bool ReadPixels(std::vector<BYTE>& bgra);
		// 알파 채널이 있는 이미지(PNG)인지. 없으면 마젠타(255, 0, 255)가 투명색이다
		bool HasAlpha() { return mbAlpha; }

		// 픽셀이 바뀔 때마다 오른다. GPU 텍스처를 다시 올릴지 판단한다.
		// SetPixel 은 알아서 올리고, GetHdc() 에 직접 GDI 로 그렸다면 MarkModified 를 부른다
		UINT GetVersion() { return mVersion; }
		void MarkModified() { mVersion++; }

		// Direct3D 11 렌더러가 만든 텍스처. Image 가 지울 때 같이 지운다
		graphics::Texture* GetTexture() { return mTexture; }
		void SetTexture(graphics::Texture* texture);

	private:
		HRESULT loadWIC(const std::wstring& path);

		HBITMAP mBitmap;
		HBITMAP mOldBitmap; // mHdc 를 만들 때 원래 선택돼 있던 비트맵. 해제 전에 되돌린다
		HDC mHdc;
		UINT mWidth;
		UINT mHeight;

		bool mbAlpha;
		UINT mVersion;
		graphics::Texture* mTexture;
	};
}
