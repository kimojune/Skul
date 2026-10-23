#include "yaImage.h"
#include "yaApplication.h"
#include "yaResources.h"
#include "yaTexture.h"
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

extern ya::Application application;

namespace ya
{
	static bool ImageIsPng(const std::wstring& path)
	{
		size_t dot = path.find_last_of(L'.');
		return dot != std::wstring::npos && _wcsicmp(path.c_str() + dot, L".png") == 0;
	}

	// WIC 팩토리는 처음 PNG 를 읽을 때 한 번 만든다
	static IWICImagingFactory* ImageWICFactory()
	{
		static Microsoft::WRL::ComPtr<IWICImagingFactory> factory = nullptr;
		if (factory == nullptr)
		{
			CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
			CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(factory.GetAddressOf()));
		}
		return factory.Get();
	}

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
		, mbAlpha(false)
		, mVersion(0)
		, mTexture(nullptr)
	{


	}

	Image::~Image()
	{
		delete mTexture;
		mTexture = nullptr;

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
		if (ImageIsPng(path))
			return loadWIC(path);

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

	HRESULT Image::loadWIC(const std::wstring& path)
	{
		IWICImagingFactory* factory = ImageWICFactory();
		if (factory == nullptr)
			return E_FAIL;

		Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder = nullptr;
		Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame = nullptr;
		Microsoft::WRL::ComPtr<IWICFormatConverter> converter = nullptr;

		HRESULT hr = factory->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ
			, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());
		if (SUCCEEDED(hr))
			hr = decoder->GetFrame(0, frame.GetAddressOf());
		if (SUCCEEDED(hr))
			hr = factory->CreateFormatConverter(converter.GetAddressOf());
		// GDI AlphaBlend 와 GPU 블렌드가 모두 premultiplied 알파를 쓰므로 PBGRA 로 바꾼다
		if (SUCCEEDED(hr))
			hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA
				, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);

		UINT width = 0;
		UINT height = 0;
		if (SUCCEEDED(hr))
			hr = converter->GetSize(&width, &height);
		if (FAILED(hr) || width == 0 || height == 0)
			return E_FAIL;

		// 위 줄부터 저장하는 32비트 DIB 에 픽셀을 바로 받는다
		BITMAPINFO info = {};
		info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		info.bmiHeader.biWidth = (LONG)width;
		info.bmiHeader.biHeight = -(LONG)height;
		info.bmiHeader.biPlanes = 1;
		info.bmiHeader.biBitCount = 32;
		info.bmiHeader.biCompression = BI_RGB;

		HDC mainDC = application.GetHdc();
		void* bits = nullptr;
		mBitmap = CreateDIBSection(mainDC, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
		if (mBitmap == nullptr)
			return E_FAIL;

		UINT stride = width * 4;
		if (FAILED(converter->CopyPixels(nullptr, stride, stride * height, (BYTE*)bits)))
			return E_FAIL;

		mWidth = width;
		mHeight = height;
		mbAlpha = true;

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
		mVersion++;
	}

	bool Image::ReadPixels(std::vector<BYTE>& bgra)
	{
		if (mBitmap == NULL || mWidth == 0 || mHeight == 0)
			return false;

		BITMAPINFO info = {};
		info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		info.bmiHeader.biWidth = (LONG)mWidth;
		info.bmiHeader.biHeight = -(LONG)mHeight; // 위 줄부터
		info.bmiHeader.biPlanes = 1;
		info.bmiHeader.biBitCount = 32;
		info.bmiHeader.biCompression = BI_RGB;

		bgra.resize((size_t)mWidth * mHeight * 4);

		// DC 에 선택된 비트맵은 GetDIBits 로 읽을 수 없으므로 잠깐 빼 둔다
		GdiFlush();
		if (mOldBitmap != NULL)
			SelectObject(mHdc, mOldBitmap);
		int lines = GetDIBits(mHdc, mBitmap, 0, mHeight, bgra.data(), &info, DIB_RGB_COLORS);
		if (mOldBitmap != NULL)
			SelectObject(mHdc, mBitmap);

		return lines == (int)mHeight;
	}

	void Image::SetTexture(graphics::Texture* texture)
	{
		if (mTexture == texture)
			return;

		delete mTexture;
		mTexture = texture;
	}
}
