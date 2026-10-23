#pragma once
#include "yaRendererBackend.h"

namespace ya
{
	// GDI 백엔드. 원래 Application과 UIDraw가 하던 GDI 그리기를 그대로 옮겼다.
	// 백버퍼 비트맵에 그리고 EndFrame에서 창으로 BitBlt 한다.
	class RendererGDI : public RendererBackend
	{
	public:
		RendererGDI();
		virtual ~RendererGDI();

		virtual bool Initialize(HWND hWnd, UINT width, UINT height) override;
		virtual void BeginFrame(COLORREF clearColor) override;
		virtual void EndFrame() override;
		virtual bool ReadPixels(std::vector<BYTE>& pixels, UINT& width, UINT& height) override;

		virtual void DrawImage(Image* image, const RenderRect& dst, const RenderRect& src
			, eImageBlend blend, BYTE alpha, bool flipX) override;
		virtual void FillRect(const RenderRect& rect, COLORREF color, BYTE alpha) override;
		virtual void DrawRect(const RenderRect& rect, COLORREF color, int thickness) override;
		virtual void DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int thickness) override;
		virtual void DrawString(const std::wstring& text, int x, int y, const RenderFont& font, COLORREF color, eTextAlign align) override;
		virtual void DrawStringBox(const std::wstring& text, const RenderRect& box, const RenderFont& font, COLORREF color, eTextAlign align) override;
		virtual SIZE MeasureString(const std::wstring& text, const RenderFont& font) override;
		virtual void ReleaseCache() override;

		virtual HDC GetHdc() override { return mBackHdc; }
		virtual const wchar_t* GetName() override { return L"GDI"; }
		virtual std::wstring GetAdapterName() override { return L"CPU (GDI)"; }

	private:
		// 좌우 반전·반투명 그리기에 쓰는 임시 비트맵. 크기가 모자라면 다시 만든다
		struct Scratch
		{
			HDC hdc = nullptr;
			HBITMAP bitmap = nullptr;
			HBITMAP oldBitmap = nullptr;
			int width = 0;
			int height = 0;
		};

		HFONT getFont(const RenderFont& font);
		HDC getScratch(Scratch& scratch, int width, int height);
		void releaseScratch(Scratch& scratch);

		HWND mHwnd;
		HDC mHdc;
		HBITMAP mBackBuffer;
		HBITMAP mBackOldBitmap;
		HDC mBackHdc;
		UINT mWidth;
		UINT mHeight;

		std::map<std::wstring, HFONT> mFonts;
		Scratch mShade;		// 1x1 색 한 점. AlphaBlend로 늘려 반투명 사각형을 그린다
		Scratch mFlip;
		Scratch mMix;
	};
}
