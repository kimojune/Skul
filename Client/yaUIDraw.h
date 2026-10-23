#pragma once
#include "YamYamEngine.h"

namespace ya
{
	class Image;

	// 화면 좌표로 그리는 UI 도우미. 카메라 위치를 쓰지 않으므로 HUD처럼 화면에 고정된 것을 그릴 때 쓴다.
	// (월드에 있는 것은 Camera::CaluatePos로 바꾼 뒤 그린다)
	// 실제 그리기는 Renderer가 한다. hdc는 예전 GDI 시절 호출과 맞추려고 남겨 둔 인자이고 쓰지 않는다.
	// GDI 백엔드는 그린 뒤 백버퍼 DC의 폰트·글자색·배경 모드·정렬을 원래대로 돌려놓는다.
	//
	// HUD 사용 예 (UI 레이어 오브젝트의 Render(HDC hdc) 안에서. 숫자는 예시):
	//   HP 바       UIDraw::DrawBar(hdc, 90, 830, 240, 18, hp / maxHp, hpBar, shownHp / maxHp); // shownHp: 천천히 따라오는 값
	//               UIDraw::DrawLabelBox(hdc, 90, 830, 240, 18, L"45 / 100", hpText);           // hpText.align = Center
	//   스컬 초상화  UIDraw::DrawImage(hdc, portrait, 16, 780, 2.0f);
	//   스킬 쿨다운  UIDraw::DrawImage(hdc, skillIcon, x, y);
	//               UIDraw::DrawCooldown(hdc, x, y, 48, 48, remain / coolTime, remain);
	//   보스 HP 바   UIDraw::DrawBar(hdc, 400, 40, 800, 20, bossHp / bossMaxHp, bossBar);
	class UIDraw
	{
	public:
		enum class eAlign
		{
			Left,
			Center,
			Right,
		};

		struct TextStyle
		{
			const wchar_t* face = L"Malgun Gothic";
			int size = 18;							// 글자 높이(px)
			COLORREF color = RGB(255, 255, 255);
			bool bold = false;
			eAlign align = eAlign::Left;
			bool shadow = false;					// 오른쪽 아래 1px 검은 그림자
		};

		struct BarStyle
		{
			COLORREF fill = RGB(214, 46, 46);		// 남은 양
			COLORREF back = RGB(36, 32, 40);		// 빈 부분
			COLORREF border = RGB(0, 0, 0);
			COLORREF trail = RGB(240, 240, 240);	// 방금 깎인 부분 (trailRatio)
			int borderWidth = 2;					// 0이면 테두리 없음
		};

		// 글자. align이 Left면 x가 왼쪽 끝, Center면 가운데, Right면 오른쪽 끝. y는 윗변
		static void DrawLabel(HDC hdc, int x, int y, const std::wstring& text, const TextStyle& style = TextStyle());
		// 상자 안에 그린다. 세로는 가운데, 가로는 style.align (바 위의 숫자, 쿨다운 초)
		static void DrawLabelBox(HDC hdc, int x, int y, int width, int height, const std::wstring& text, const TextStyle& style = TextStyle());
		static SIZE MeasureLabel(HDC hdc, const std::wstring& text, const TextStyle& style = TextStyle());

		// 막대. 왼쪽부터 ratio(0~1)만큼 채운다. trailRatio가 ratio보다 크면 그 사이를 trail 색으로 칠한다
		static void DrawBar(HDC hdc, int x, int y, int width, int height, float ratio
			, const BarStyle& style = BarStyle(), float trailRatio = -1.0f);

		// 꽉 찬 사각형(테두리 포함)과 반투명 사각형 (패널 배경, 쿨다운 덮개)
		static void DrawBox(HDC hdc, int x, int y, int width, int height, COLORREF fill, COLORREF border, int borderWidth = 1);
		static void DrawShade(HDC hdc, int x, int y, int width, int height, COLORREF color, BYTE alpha);

		// 이미지. 마젠타(255, 0, 255)는 투명. image가 nullptr이면 그리지 않는다
		static void DrawImage(HDC hdc, Image* image, int x, int y, float scale = 1.0f);
		// 스프라이트 시트의 일부(src)를 (x, y, width, height)에 늘려 그린다
		static void DrawImagePart(HDC hdc, Image* image, int x, int y, int width, int height
			, int srcX, int srcY, int srcWidth, int srcHeight);

		// 쿨다운 덮개. 위에서부터 remainRatio만큼 어둡게 덮고(다 차면 아래부터 드러남), remainSeconds > 0이면 가운데에 남은 초를 쓴다
		static void DrawCooldown(HDC hdc, int x, int y, int width, int height, float remainRatio, float remainSeconds = 0.0f);

		// 캐시한 폰트와 반투명용 DC를 지운다. 프로그램이 끝날 때 자동으로도 불린다
		static void Release();

	private:
		static void fillRect(HDC hdc, int x, int y, int width, int height, COLORREF color);
	};
}
