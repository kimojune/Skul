#pragma once
#include "yaRenderer.h"

namespace ya
{
	// Renderer가 부르는 그리기 백엔드. GDI(RendererGDI)와 Direct3D 11(RendererDX11)이 있다.
	class RendererBackend
	{
	public:
		virtual ~RendererBackend() {}

		virtual bool Initialize(HWND hWnd, UINT width, UINT height) = 0;
		virtual void BeginFrame(COLORREF clearColor) = 0;
		virtual void EndFrame() = 0;
		// 마지막 화면을 24비트 BMP 픽셀 배열로 읽는다 (아래 줄부터, 한 줄은 4바이트 단위로 맞춤)
		virtual bool ReadPixels(std::vector<BYTE>& pixels, UINT& width, UINT& height) = 0;

		virtual void DrawImage(Image* image, const RenderRect& dst, const RenderRect& src
			, eImageBlend blend, BYTE alpha, bool flipX, BYTE flash) = 0;
		virtual void FillRect(const RenderRect& rect, COLORREF color, BYTE alpha) = 0;
		virtual void DrawRect(const RenderRect& rect, COLORREF color, int thickness) = 0;
		virtual void DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int thickness) = 0;
		virtual void DrawString(const std::wstring& text, int x, int y, const RenderFont& font, COLORREF color, eTextAlign align) = 0;
		virtual void DrawStringBox(const std::wstring& text, const RenderRect& box, const RenderFont& font, COLORREF color, eTextAlign align) = 0;
		virtual SIZE MeasureString(const std::wstring& text, const RenderFont& font) = 0;
		virtual void ReleaseCache() = 0;

		virtual HDC GetHdc() = 0;
		virtual const wchar_t* GetName() = 0;
		virtual std::wstring GetAdapterName() = 0;

		// 이번 프레임에 그린 양. EndFrame 뒤에 Renderer가 읽고 비운다
		RenderStats& GetStats() { return mStats; }

	protected:
		RenderStats mStats;
	};
}
