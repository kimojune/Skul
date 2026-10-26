#include "yaUIDraw.h"
#include "yaImage.h"
#include "yaRenderer.h"

namespace ya
{
	// 종료 때 Renderer가 캐시한 폰트를 지우기 위한 전역 객체
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

	static RenderFont UIDrawFont(const UIDraw::TextStyle& style)
	{
		RenderFont font;
		font.face = style.face;
		font.size = style.size;
		font.bold = style.bold;
		return font;
	}

	static eTextAlign UIDrawAlign(UIDraw::eAlign align)
	{
		if (align == UIDraw::eAlign::Center)
			return eTextAlign::Center;
		if (align == UIDraw::eAlign::Right)
			return eTextAlign::Right;
		return eTextAlign::Left;
	}

	void UIDraw::DrawLabel(HDC hdc, int x, int y, const std::wstring& text, const TextStyle& style)
	{
		if (text.empty())
			return;

		RenderFont font = UIDrawFont(style);
		eTextAlign align = UIDrawAlign(style.align);
		if (style.shadow)
			Renderer::DrawString(text, x + 1, y + 1, font, RGB(0, 0, 0), align);
		Renderer::DrawString(text, x, y, font, style.color, align);
	}

	void UIDraw::DrawLabelBox(HDC hdc, int x, int y, int width, int height, const std::wstring& text, const TextStyle& style)
	{
		if (text.empty())
			return;

		RenderFont font = UIDrawFont(style);
		eTextAlign align = UIDrawAlign(style.align);
		if (style.shadow)
			Renderer::DrawStringBox(text, x + 1, y + 1, width, height, font, RGB(0, 0, 0), align);
		Renderer::DrawStringBox(text, x, y, width, height, font, style.color, align);
	}

	SIZE UIDraw::MeasureLabel(HDC hdc, const std::wstring& text, const TextStyle& style)
	{
		return Renderer::MeasureString(text, UIDrawFont(style));
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

		Renderer::FillRect(x, y, width, height, color, alpha);
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

		Renderer::DrawImage(image, x, y, width, height, srcX, srcY, srcWidth, srcHeight);
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
		Renderer::ReleaseCache();
	}

	void UIDraw::fillRect(HDC hdc, int x, int y, int width, int height, COLORREF color)
	{
		if (width <= 0 || height <= 0)
			return;

		Renderer::FillRect(x, y, width, height, color);
	}
}
