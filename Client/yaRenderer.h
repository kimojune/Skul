#pragma once
#include "YamYamEngine.h"

namespace ya
{
	class Image;
	class RendererBackend;

	enum class eRendererType
	{
		GDI,
		DX11,
		End,
	};

	// 이미지를 바닥에 섞는 방법
	enum class eImageBlend
	{
		ColorKey,		// 마젠타(255, 0, 255)만 빼고 덮어 그린다 (TransparentBlt). 알파가 있는 이미지(PNG)는 알파로 섞는다
		AlphaChannel,	// 이미지의 알파 채널(premultiplied)로 섞는다 (AlphaBlend + AC_SRC_ALPHA)
	};

	enum class eTextAlign
	{
		Left,
		Center,
		Right,
	};

	struct RenderRect
	{
		int x;
		int y;
		int width;
		int height;
	};

	struct RenderFont
	{
		const wchar_t* face = L"Malgun Gothic";
		int size = 18;		// 글자 높이(px)
		bool bold = false;
	};

	// 한 프레임 동안 그린 양. 디버그 정보창에 보여준다
	struct RenderStats
	{
		UINT drawCalls = 0;	// GPU에 보낸 그리기 명령 수 (GDI는 그리기 함수 호출 수)
		UINT sprites = 0;	// 그린 이미지 수
		UINT shapes = 0;	// 사각형·선·글자 수
	};

	// 모든 그리기의 입구. Render(HDC) 함수들은 GDI 함수 대신 이 함수들을 부른다.
	// 실제로 그리는 쪽(백엔드)은 GDI와 Direct3D 11 중에서 시작할 때 고른다 (--renderer gdi|dx11).
	// 좌표는 모두 화면 픽셀이다 (왼쪽 위가 0, 0). 월드 좌표는 Camera::CaluatePos로 바꾼 뒤 넘긴다.
	class Renderer
	{
	public:
		// 실패하면 false. 이때는 다른 백엔드로 다시 부를 수 있다.
		// forceWarp: DX11 을 그래픽 카드 대신 WARP(CPU 소프트웨어 래스터라이저)로 만든다 (확인용)
		static bool Initialize(eRendererType type, HWND hWnd, UINT width, UINT height, bool forceWarp = false);
		static void Release();

		static void BeginFrame(COLORREF clearColor);
		static void EndFrame();	// 화면에 내보낸다 (present)
		// 마지막으로 그린 화면을 24비트 BMP로 저장한다
		static bool SaveScreenshot(const std::wstring& path);

		// 이미지의 (srcX, srcY, srcWidth, srcHeight) 부분을 (x, y, width, height)에 늘려 그린다.
		// alpha는 전체 투명도(0 투명 ~ 255 불투명), flipX는 좌우 반전
		static void DrawImage(Image* image, int x, int y, int width, int height
			, int srcX, int srcY, int srcWidth, int srcHeight
			, eImageBlend blend = eImageBlend::ColorKey, BYTE alpha = 255, bool flipX = false);
		static void FillRect(int x, int y, int width, int height, COLORREF color, BYTE alpha = 255);
		static void DrawRect(int x, int y, int width, int height, COLORREF color, int thickness = 1);
		static void DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int thickness = 1);

		// 글자. align이 Left면 x가 왼쪽 끝, Center면 가운데, Right면 오른쪽 끝. y는 윗변
		static void DrawString(const std::wstring& text, int x, int y, const RenderFont& font, COLORREF color, eTextAlign align = eTextAlign::Left);
		// 상자 안에 한 줄로 그린다. 세로는 가운데, 가로는 align
		static void DrawStringBox(const std::wstring& text, int x, int y, int width, int height, const RenderFont& font, COLORREF color, eTextAlign align = eTextAlign::Left);
		static SIZE MeasureString(const std::wstring& text, const RenderFont& font);
		// 캐시한 글꼴 등을 지운다
		static void ReleaseCache();

		static eRendererType GetType() { return mType; }
		static const wchar_t* GetName();
		static std::wstring GetAdapterName();
		static const RenderStats& GetLastFrameStats() { return mLastStats; }
		// Render(HDC)에 넘길 DC. GDI는 백버퍼 DC, DX11은 화면에 나오지 않는 작은 메모리 DC
		static HDC GetHdc();

	private:
		static RendererBackend* mBackend;
		static eRendererType mType;
		static RenderStats mLastStats;
	};
}
