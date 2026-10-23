#include "yaRendererGDI.h"
#include "yaImage.h"

namespace ya
{
	static const COLORREF RendererGDIColorKey = RGB(255, 0, 255);

	// 글자를 그리는 동안 DC의 폰트·배경 모드·글자색·정렬을 바꿨다가 돌려놓는다
	struct RendererGDITextScope
	{
		RendererGDITextScope(HDC hdc, HFONT font, COLORREF color, UINT align)
			: mHdc(hdc)
		{
			mOldFont = (HFONT)SelectObject(hdc, font);
			mOldMode = SetBkMode(hdc, TRANSPARENT);
			mOldColor = SetTextColor(hdc, color);
			mOldAlign = SetTextAlign(hdc, align);
		}
		~RendererGDITextScope()
		{
			SetTextAlign(mHdc, mOldAlign);
			SetTextColor(mHdc, mOldColor);
			SetBkMode(mHdc, mOldMode);
			SelectObject(mHdc, mOldFont);
		}

		HDC mHdc;
		HFONT mOldFont;
		int mOldMode;
		COLORREF mOldColor;
		UINT mOldAlign;
	};

	RendererGDI::RendererGDI()
		: mHwnd(nullptr)
		, mHdc(nullptr)
		, mBackBuffer(nullptr)
		, mBackOldBitmap(nullptr)
		, mBackHdc(nullptr)
		, mWidth(0)
		, mHeight(0)
	{
	}

	RendererGDI::~RendererGDI()
	{
		ReleaseCache();

		if (mBackHdc != nullptr)
		{
			SelectObject(mBackHdc, mBackOldBitmap);
			DeleteDC(mBackHdc);
		}
		if (mBackBuffer != nullptr)
			DeleteObject(mBackBuffer);
		if (mHdc != nullptr)
			ReleaseDC(mHwnd, mHdc);
	}

	bool RendererGDI::Initialize(HWND hWnd, UINT width, UINT height)
	{
		mHwnd = hWnd;
		mHdc = GetDC(hWnd);
		mWidth = width;
		mHeight = height;

		mBackBuffer = CreateCompatibleBitmap(mHdc, mWidth, mHeight);
		mBackHdc = CreateCompatibleDC(mHdc);
		if (mBackBuffer == nullptr || mBackHdc == nullptr)
			return false;

		mBackOldBitmap = (HBITMAP)SelectObject(mBackHdc, mBackBuffer);
		return true;
	}

	void RendererGDI::BeginFrame(COLORREF clearColor)
	{
		HBRUSH clearBrush = CreateSolidBrush(clearColor);
		HBRUSH oldBrush = (HBRUSH)SelectObject(mBackHdc, clearBrush);
		Rectangle(mBackHdc, -1, -1, mWidth + 2, mHeight + 2);
		SelectObject(mBackHdc, oldBrush);
		DeleteObject(clearBrush);
		mStats.drawCalls++;
	}

	void RendererGDI::EndFrame()
	{
		// 백버퍼에 있는 그림을 원본버퍼에 그려줘야한다.
		BitBlt(mHdc, 0, 0, mWidth, mHeight, mBackHdc, 0, 0, SRCCOPY);
		mStats.drawCalls++;
	}

	bool RendererGDI::ReadPixels(std::vector<BYTE>& pixels, UINT& width, UINT& height)
	{
		// 백버퍼는 DC에 선택된 상태라 직접 읽지 않고 DIB에 복사해서 읽는다.
		BITMAPINFO info = {};
		info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		info.bmiHeader.biWidth = mWidth;
		info.bmiHeader.biHeight = mHeight;
		info.bmiHeader.biPlanes = 1;
		info.bmiHeader.biBitCount = 24;
		info.bmiHeader.biCompression = BI_RGB;

		void* bits = nullptr;
		HBITMAP dib = CreateDIBSection(mBackHdc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
		if (dib == nullptr)
			return false;

		HDC dibDC = CreateCompatibleDC(mBackHdc);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(dibDC, dib);
		BitBlt(dibDC, 0, 0, mWidth, mHeight, mBackHdc, 0, 0, SRCCOPY);
		GdiFlush();

		DWORD imageSize = ((mWidth * 3 + 3) / 4) * 4 * mHeight;
		pixels.assign((BYTE*)bits, (BYTE*)bits + imageSize);
		width = mWidth;
		height = mHeight;

		SelectObject(dibDC, oldBitmap);
		DeleteDC(dibDC);
		DeleteObject(dib);
		return true;
	}

	void RendererGDI::DrawImage(Image* image, const RenderRect& dst, const RenderRect& src
		, eImageBlend blend, BYTE alpha, bool flipX)
	{
		HDC srcHdc = image->GetHdc();
		int srcX = src.x;
		int srcY = src.y;

		// PNG 처럼 알파가 있는 이미지는 마젠타 키 대신 알파로 섞는다
		if (image->HasAlpha())
			blend = eImageBlend::AlphaChannel;

		if (flipX)
		{
			// TransparentBlt·AlphaBlend는 뒤집어 그리지 못하므로 좌우를 바꾼 사본에서 그린다
			HDC flipHdc = getScratch(mFlip, src.width, src.height);
			StretchBlt(flipHdc, src.width - 1, 0, -src.width, src.height
				, srcHdc, src.x, src.y, src.width, src.height, SRCCOPY);
			srcHdc = flipHdc;
			srcX = 0;
			srcY = 0;
			mStats.drawCalls++;
		}

		if (blend == eImageBlend::AlphaChannel)
		{
			BLENDFUNCTION func = {};
			func.BlendOp = AC_SRC_OVER;
			func.BlendFlags = 0;
			func.AlphaFormat = AC_SRC_ALPHA;
			func.SourceConstantAlpha = alpha; // 0(투명) ~ 255(불투명) 알파값

			AlphaBlend(mBackHdc, dst.x, dst.y, dst.width, dst.height
				, srcHdc, srcX, srcY, src.width, src.height, func);
			mStats.drawCalls++;
			return;
		}

		if (alpha == 255)
		{
			TransparentBlt(mBackHdc, dst.x, dst.y, dst.width, dst.height
				, srcHdc, srcX, srcY, src.width, src.height, RendererGDIColorKey);
			mStats.drawCalls++;
			return;
		}

		// 반투명: 바닥을 복사한 임시 비트맵에 마젠타를 뺀 그림을 덮은 뒤, 그 결과를 바닥에 알파로 섞는다
		HDC mixHdc = getScratch(mMix, dst.width, dst.height);
		BitBlt(mixHdc, 0, 0, dst.width, dst.height, mBackHdc, dst.x, dst.y, SRCCOPY);
		TransparentBlt(mixHdc, 0, 0, dst.width, dst.height
			, srcHdc, srcX, srcY, src.width, src.height, RendererGDIColorKey);

		BLENDFUNCTION func = {};
		func.BlendOp = AC_SRC_OVER;
		func.SourceConstantAlpha = alpha;
		AlphaBlend(mBackHdc, dst.x, dst.y, dst.width, dst.height
			, mixHdc, 0, 0, dst.width, dst.height, func);
		mStats.drawCalls += 3;
	}

	void RendererGDI::FillRect(const RenderRect& rect, COLORREF color, BYTE alpha)
	{
		if (alpha == 255)
		{
			RECT area = { rect.x, rect.y, rect.x + rect.width, rect.y + rect.height };
			HBRUSH brush = CreateSolidBrush(color);
			::FillRect(mBackHdc, &area, brush);
			DeleteObject(brush);
			mStats.drawCalls++;
			return;
		}

		// 1x1 비트맵 한 점을 색으로 칠하고 AlphaBlend로 늘려 그린다
		HDC shadeHdc = getScratch(mShade, 1, 1);
		::SetPixel(shadeHdc, 0, 0, color);

		BLENDFUNCTION blend = {};
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = alpha;
		AlphaBlend(mBackHdc, rect.x, rect.y, rect.width, rect.height, shadeHdc, 0, 0, 1, 1, blend);
		mStats.drawCalls++;
	}

	void RendererGDI::DrawRect(const RenderRect& rect, COLORREF color, int thickness)
	{
		HPEN pen = CreatePen(PS_SOLID, thickness, color);
		HPEN oldPen = (HPEN)SelectObject(mBackHdc, pen);
		HBRUSH oldBrush = (HBRUSH)SelectObject(mBackHdc, GetStockObject(NULL_BRUSH));

		Rectangle(mBackHdc, rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);

		SelectObject(mBackHdc, oldBrush);
		SelectObject(mBackHdc, oldPen);
		DeleteObject(pen);
		mStats.drawCalls++;
	}

	void RendererGDI::DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int thickness)
	{
		HPEN pen = CreatePen(PS_SOLID, thickness, color);
		HPEN oldPen = (HPEN)SelectObject(mBackHdc, pen);

		MoveToEx(mBackHdc, x1, y1, NULL);
		LineTo(mBackHdc, x2, y2);

		SelectObject(mBackHdc, oldPen);
		DeleteObject(pen);
		mStats.drawCalls++;
	}

	void RendererGDI::DrawString(const std::wstring& text, int x, int y, const RenderFont& font, COLORREF color, eTextAlign align)
	{
		UINT gdiAlign = TA_TOP | TA_NOUPDATECP;
		if (align == eTextAlign::Center)
			gdiAlign |= TA_CENTER;
		else if (align == eTextAlign::Right)
			gdiAlign |= TA_RIGHT;
		else
			gdiAlign |= TA_LEFT;

		RendererGDITextScope scope(mBackHdc, getFont(font), color, gdiAlign);
		TextOutW(mBackHdc, x, y, text.c_str(), (int)text.size());
		mStats.drawCalls++;
	}

	void RendererGDI::DrawStringBox(const std::wstring& text, const RenderRect& box, const RenderFont& font, COLORREF color, eTextAlign align)
	{
		UINT format = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_NOCLIP;
		if (align == eTextAlign::Center)
			format |= DT_CENTER;
		else if (align == eTextAlign::Right)
			format |= DT_RIGHT;
		else
			format |= DT_LEFT;

		RendererGDITextScope scope(mBackHdc, getFont(font), color, TA_LEFT | TA_TOP | TA_NOUPDATECP);
		RECT rect = { box.x, box.y, box.x + box.width, box.y + box.height };
		DrawTextW(mBackHdc, text.c_str(), (int)text.size(), &rect, format);
		mStats.drawCalls++;
	}

	SIZE RendererGDI::MeasureString(const std::wstring& text, const RenderFont& font)
	{
		SIZE size = {};
		RendererGDITextScope scope(mBackHdc, getFont(font), GetTextColor(mBackHdc), TA_LEFT | TA_TOP | TA_NOUPDATECP);
		GetTextExtentPoint32W(mBackHdc, text.c_str(), (int)text.size(), &size);
		return size;
	}

	void RendererGDI::ReleaseCache()
	{
		for (auto& font : mFonts)
			DeleteObject(font.second);
		mFonts.clear();

		releaseScratch(mShade);
		releaseScratch(mFlip);
		releaseScratch(mMix);
	}

	HFONT RendererGDI::getFont(const RenderFont& font)
	{
		std::wstring key = std::wstring(font.face) + L"|" + std::to_wstring(font.size) + (font.bold ? L"|b" : L"");
		auto iter = mFonts.find(key);
		if (iter != mFonts.end())
			return iter->second;

		// 높이를 음수로 주면 셀 높이가 아니라 글자 높이 기준이 된다
		HFONT hfont = CreateFontW(-font.size, 0, 0, 0, font.bold ? FW_BOLD : FW_NORMAL
			, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
			, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, font.face);
		if (hfont == nullptr)
			return (HFONT)GetStockObject(DEFAULT_GUI_FONT);

		mFonts.insert(std::make_pair(key, hfont));
		return hfont;
	}

	HDC RendererGDI::getScratch(Scratch& scratch, int width, int height)
	{
		if (scratch.hdc != nullptr && scratch.width >= width && scratch.height >= height)
			return scratch.hdc;

		int newWidth = width > scratch.width ? width : scratch.width;
		int newHeight = height > scratch.height ? height : scratch.height;
		releaseScratch(scratch);

		scratch.hdc = CreateCompatibleDC(mBackHdc);
		scratch.bitmap = CreateCompatibleBitmap(mBackHdc, newWidth, newHeight);
		scratch.oldBitmap = (HBITMAP)SelectObject(scratch.hdc, scratch.bitmap);
		scratch.width = newWidth;
		scratch.height = newHeight;
		return scratch.hdc;
	}

	void RendererGDI::releaseScratch(Scratch& scratch)
	{
		if (scratch.hdc == nullptr)
			return;

		SelectObject(scratch.hdc, scratch.oldBitmap);
		DeleteObject(scratch.bitmap);
		DeleteDC(scratch.hdc);
		scratch = Scratch();
	}
}
