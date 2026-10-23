#include "yaUIDraw.h"
#include "yaImage.h"

namespace ya
{
	std::map<std::wstring, HFONT> UIDraw::mFonts;
	HDC UIDraw::mShadeHdc = nullptr;
	HBITMAP UIDraw::mShadeBitmap = nullptr;
	HBITMAP UIDraw::mShadeOldBitmap = nullptr;

	// Application 소멸자를 거치지 않고도 종료 때 폰트를 지우기 위한 전역 객체 (mFonts보다 먼저 소멸)
	struct UIDrawReleaseAtExit
	{
		~UIDrawReleaseAtExit() { UIDraw::Release(); }
	};
	static UIDrawReleaseAtExit UIDrawReleaseAtExitInstance;

	// NaN은 0으로 본다
	static float UIDrawClamp01(float value)
	{
		if (!(value > 0.0f))
			return 0.0f;
		if (value > 1.0f)
			return 1.0f;
		return value;
	}

	// 글자를 그리는 동안 DC의 폰트·배경 모드·글자색·정렬을 바꿨다가 돌려놓는다
	struct UIDrawTextScope
	{
		UIDrawTextScope(HDC hdc, HFONT font, UINT align)
			: mHdc(hdc)
		{
			mOldFont = (HFONT)SelectObject(hdc, font);
			mOldMode = SetBkMode(hdc, TRANSPARENT);
			mOldColor = GetTextColor(hdc);
			mOldAlign = SetTextAlign(hdc, align);
		}
		~UIDrawTextScope()
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

	void UIDraw::DrawLabel(HDC hdc, int x, int y, const std::wstring& text, const TextStyle& style)
	{
		if (text.empty())
			return;

		UINT align = TA_TOP | TA_NOUPDATECP;
		if (style.align == eAlign::Center)
			align |= TA_CENTER;
		else if (style.align == eAlign::Right)
			align |= TA_RIGHT;
		else
			align |= TA_LEFT;

		UIDrawTextScope scope(hdc, getFont(style), align);
		if (style.shadow)
		{
			SetTextColor(hdc, RGB(0, 0, 0));
			TextOutW(hdc, x + 1, y + 1, text.c_str(), (int)text.size());
		}
		SetTextColor(hdc, style.color);
		TextOutW(hdc, x, y, text.c_str(), (int)text.size());
	}

	void UIDraw::DrawLabelBox(HDC hdc, int x, int y, int width, int height, const std::wstring& text, const TextStyle& style)
	{
		if (text.empty())
			return;

		UINT format = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_NOCLIP;
		if (style.align == eAlign::Center)
			format |= DT_CENTER;
		else if (style.align == eAlign::Right)
			format |= DT_RIGHT;
		else
			format |= DT_LEFT;

		UIDrawTextScope scope(hdc, getFont(style), TA_LEFT | TA_TOP | TA_NOUPDATECP);
		if (style.shadow)
		{
			RECT shadowRect = { x + 1, y + 1, x + width + 1, y + height + 1 };
			SetTextColor(hdc, RGB(0, 0, 0));
			DrawTextW(hdc, text.c_str(), (int)text.size(), &shadowRect, format);
		}
		RECT rect = { x, y, x + width, y + height };
		SetTextColor(hdc, style.color);
		DrawTextW(hdc, text.c_str(), (int)text.size(), &rect, format);
	}

	SIZE UIDraw::MeasureLabel(HDC hdc, const std::wstring& text, const TextStyle& style)
	{
		SIZE size = {};
		UIDrawTextScope scope(hdc, getFont(style), TA_LEFT | TA_TOP | TA_NOUPDATECP);
		GetTextExtentPoint32W(hdc, text.c_str(), (int)text.size(), &size);
		return size;
	}

	void UIDraw::DrawBar(HDC hdc, int x, int y, int width, int height, float ratio, const BarStyle& style, float trailRatio)
	{
		int border = style.borderWidth > 0 ? style.borderWidth : 0;
		int innerWidth = width - border * 2;
		int innerHeight = height - border * 2;
		if (innerWidth <= 0 || innerHeight <= 0)
			return;

		// 테두리 -> 빈 부분 -> 깎인 부분 -> 남은 양 순서로 덮어 그린다
		if (border > 0)
			fillRect(hdc, x, y, width, height, style.border);
		fillRect(hdc, x + border, y + border, innerWidth, innerHeight, style.back);

		ratio = UIDrawClamp01(ratio);
		trailRatio = UIDrawClamp01(trailRatio);
		if (trailRatio > ratio)
			fillRect(hdc, x + border, y + border, (int)(innerWidth * trailRatio + 0.5f), innerHeight, style.trail);
		fillRect(hdc, x + border, y + border, (int)(innerWidth * ratio + 0.5f), innerHeight, style.fill);
	}

	void UIDraw::DrawBox(HDC hdc, int x, int y, int width, int height, COLORREF fill, COLORREF border, int borderWidth)
	{
		if (borderWidth > 0)
		{
			fillRect(hdc, x, y, width, height, border);
			fillRect(hdc, x + borderWidth, y + borderWidth, width - borderWidth * 2, height - borderWidth * 2, fill);
		}
		else
		{
			fillRect(hdc, x, y, width, height, fill);
		}
	}

	void UIDraw::DrawShade(HDC hdc, int x, int y, int width, int height, COLORREF color, BYTE alpha)
	{
		if (width <= 0 || height <= 0 || alpha == 0)
			return;
		if (alpha == 255)
		{
			fillRect(hdc, x, y, width, height, color);
			return;
		}

		// 1x1 비트맵 한 점을 색으로 칠하고 AlphaBlend로 늘려 그린다
		if (mShadeHdc == nullptr)
		{
			mShadeHdc = CreateCompatibleDC(hdc);
			mShadeBitmap = CreateCompatibleBitmap(hdc, 1, 1);
			mShadeOldBitmap = (HBITMAP)SelectObject(mShadeHdc, mShadeBitmap);
		}
		::SetPixel(mShadeHdc, 0, 0, color);

		BLENDFUNCTION blend = {};
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = alpha;
		AlphaBlend(hdc, x, y, width, height, mShadeHdc, 0, 0, 1, 1, blend);
	}

	void UIDraw::DrawImage(HDC hdc, Image* image, int x, int y, float scale)
	{
		if (image == nullptr)
			return;

		int width = (int)image->GetWidth();
		int height = (int)image->GetHeight();
		DrawImagePart(hdc, image, x, y, (int)(width * scale + 0.5f), (int)(height * scale + 0.5f), 0, 0, width, height);
	}

	void UIDraw::DrawImagePart(HDC hdc, Image* image, int x, int y, int width, int height
		, int srcX, int srcY, int srcWidth, int srcHeight)
	{
		if (image == nullptr || width <= 0 || height <= 0 || srcWidth <= 0 || srcHeight <= 0)
			return;

		TransparentBlt(hdc, x, y, width, height
			, image->GetHdc(), srcX, srcY, srcWidth, srcHeight
			, RGB(255, 0, 255));
	}

	void UIDraw::DrawCooldown(HDC hdc, int x, int y, int width, int height, float remainRatio, float remainSeconds)
	{
		remainRatio = UIDrawClamp01(remainRatio);
		if (remainRatio <= 0.0f)
			return;

		DrawShade(hdc, x, y, width, (int)(height * remainRatio + 0.5f), RGB(0, 0, 0), 160);

		if (remainSeconds > 0.0f)
		{
			wchar_t text[16] = {};
			if (remainSeconds >= 1.0f)
				swprintf_s(text, L"%d", (int)ceilf(remainSeconds));
			else
				swprintf_s(text, L"%.1f", remainSeconds);

			TextStyle style;
			style.size = height / 2 > 12 ? height / 2 : 12;
			style.bold = true;
			style.align = eAlign::Center;
			style.shadow = true;
			DrawLabelBox(hdc, x, y, width, height, text, style);
		}
	}

	void UIDraw::Release()
	{
		for (auto& font : mFonts)
			DeleteObject(font.second);
		mFonts.clear();

		if (mShadeHdc != nullptr)
		{
			SelectObject(mShadeHdc, mShadeOldBitmap);
			DeleteObject(mShadeBitmap);
			DeleteDC(mShadeHdc);
			mShadeHdc = nullptr;
			mShadeBitmap = nullptr;
			mShadeOldBitmap = nullptr;
		}
	}

	HFONT UIDraw::getFont(const TextStyle& style)
	{
		std::wstring key = std::wstring(style.face) + L"|" + std::to_wstring(style.size) + (style.bold ? L"|b" : L"");
		auto iter = mFonts.find(key);
		if (iter != mFonts.end())
			return iter->second;

		// 높이를 음수로 주면 셀 높이가 아니라 글자 높이 기준이 된다
		HFONT font = CreateFontW(-style.size, 0, 0, 0, style.bold ? FW_BOLD : FW_NORMAL
			, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
			, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, style.face);
		if (font == nullptr)
			return (HFONT)GetStockObject(DEFAULT_GUI_FONT);

		mFonts.insert(std::make_pair(key, font));
		return font;
	}

	void UIDraw::fillRect(HDC hdc, int x, int y, int width, int height, COLORREF color)
	{
		if (width <= 0 || height <= 0)
			return;

		RECT rect = { x, y, x + width, y + height };
		HBRUSH brush = CreateSolidBrush(color);
		FillRect(hdc, &rect, brush);
		DeleteObject(brush);
	}
}
