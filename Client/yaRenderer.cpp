#include "yaRenderer.h"
#include "yaRendererBackend.h"
#include "yaRendererGDI.h"
#include "yaRendererDX11.h"
#include "yaImage.h"

namespace ya
{
	RendererBackend* Renderer::mBackend = nullptr;
	eRendererType Renderer::mType = eRendererType::GDI;
	RenderStats Renderer::mLastStats = {};

	bool Renderer::Initialize(eRendererType type, HWND hWnd, UINT width, UINT height, bool forceWarp)
	{
		Release();

		RendererBackend* backend = nullptr;
		switch (type)
		{
		case eRendererType::GDI:
			backend = new RendererGDI();
			break;
		case eRendererType::DX11:
			backend = new RendererDX11(forceWarp);
			break;
		default:
			return false;
		}

		if (!backend->Initialize(hWnd, width, height))
		{
			delete backend;
			return false;
		}

		mBackend = backend;
		mType = type;
		return true;
	}

	void Renderer::Release()
	{
		delete mBackend;
		mBackend = nullptr;
		mLastStats = {};
	}

	void Renderer::BeginFrame(COLORREF clearColor)
	{
		if (mBackend == nullptr)
			return;

		mBackend->GetStats() = {};
		mBackend->BeginFrame(clearColor);
	}

	void Renderer::EndFrame()
	{
		if (mBackend == nullptr)
			return;

		mBackend->EndFrame();
		mLastStats = mBackend->GetStats();
	}

	bool Renderer::SaveScreenshot(const std::wstring& path)
	{
		if (mBackend == nullptr)
			return false;

		// 백엔드와 상관없이 같은 형식: 24비트 BI_RGB, 아래 줄부터
		std::vector<BYTE> pixels;
		UINT width = 0;
		UINT height = 0;
		if (!mBackend->ReadPixels(pixels, width, height))
			return false;

		BITMAPINFOHEADER info = {};
		info.biSize = sizeof(BITMAPINFOHEADER);
		info.biWidth = width;
		info.biHeight = height;
		info.biPlanes = 1;
		info.biBitCount = 24;
		info.biCompression = BI_RGB;

		DWORD imageSize = ((width * 3 + 3) / 4) * 4 * height;
		if (pixels.size() < imageSize)
			return false;

		BITMAPFILEHEADER fileHeader = {};
		fileHeader.bfType = 0x4D42; // "BM"
		fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fileHeader.bfSize = fileHeader.bfOffBits + imageSize;

		FILE* file = nullptr;
		_wfopen_s(&file, path.c_str(), L"wb");
		if (file == nullptr)
			return false;

		fwrite(&fileHeader, sizeof(fileHeader), 1, file);
		fwrite(&info, sizeof(BITMAPINFOHEADER), 1, file);
		fwrite(pixels.data(), imageSize, 1, file);
		fclose(file);
		return true;
	}

	void Renderer::DrawImage(Image* image, int x, int y, int width, int height
		, int srcX, int srcY, int srcWidth, int srcHeight
		, eImageBlend blend, BYTE alpha, bool flipX)
	{
		if (mBackend == nullptr || image == nullptr || alpha == 0)
			return;
		if (width <= 0 || height <= 0 || srcWidth <= 0 || srcHeight <= 0)
			return;

		mBackend->GetStats().sprites++;
		mBackend->DrawImage(image, { x, y, width, height }, { srcX, srcY, srcWidth, srcHeight }, blend, alpha, flipX);
	}

	void Renderer::FillRect(int x, int y, int width, int height, COLORREF color, BYTE alpha)
	{
		if (mBackend == nullptr || width <= 0 || height <= 0 || alpha == 0)
			return;

		mBackend->GetStats().shapes++;
		mBackend->FillRect({ x, y, width, height }, color, alpha);
	}

	void Renderer::DrawRect(int x, int y, int width, int height, COLORREF color, int thickness)
	{
		if (mBackend == nullptr || width <= 0 || height <= 0 || thickness <= 0)
			return;

		mBackend->GetStats().shapes++;
		mBackend->DrawRect({ x, y, width, height }, color, thickness);
	}

	void Renderer::DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int thickness)
	{
		if (mBackend == nullptr || thickness <= 0)
			return;

		mBackend->GetStats().shapes++;
		mBackend->DrawLine(x1, y1, x2, y2, color, thickness);
	}

	void Renderer::DrawString(const std::wstring& text, int x, int y, const RenderFont& font, COLORREF color, eTextAlign align)
	{
		if (mBackend == nullptr || text.empty())
			return;

		mBackend->GetStats().shapes++;
		mBackend->DrawString(text, x, y, font, color, align);
	}

	void Renderer::DrawStringBox(const std::wstring& text, int x, int y, int width, int height, const RenderFont& font, COLORREF color, eTextAlign align)
	{
		if (mBackend == nullptr || text.empty())
			return;

		mBackend->GetStats().shapes++;
		mBackend->DrawStringBox(text, { x, y, width, height }, font, color, align);
	}

	SIZE Renderer::MeasureString(const std::wstring& text, const RenderFont& font)
	{
		if (mBackend == nullptr)
			return SIZE{};

		return mBackend->MeasureString(text, font);
	}

	void Renderer::ReleaseCache()
	{
		if (mBackend != nullptr)
			mBackend->ReleaseCache();
	}

	const wchar_t* Renderer::GetName()
	{
		return mBackend != nullptr ? mBackend->GetName() : L"-";
	}

	std::wstring Renderer::GetAdapterName()
	{
		return mBackend != nullptr ? mBackend->GetAdapterName() : L"-";
	}

	HDC Renderer::GetHdc()
	{
		return mBackend != nullptr ? mBackend->GetHdc() : nullptr;
	}
}
