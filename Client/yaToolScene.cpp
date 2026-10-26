#include "yaToolScene.h"
#include "yaApplication.h"
#include "yaImage.h"
#include "yaResources.h"
#include "yaTile.h"
#include "yaObject.h"
#include "yaInput.h"
#include "yaTilePalatte.h"
#include "yaCamera.h"
#include "yaScenario.h"

extern ya::Application application;

namespace ya
{
    // 아틀라스 창에 그리는 배율. 아틀라스(1024x1024)가 모니터보다 크면 줄여서 보여주고, 클릭 좌표는 이 배율로 되돌린다 (AtlasWndProc)
    float ToolSceneAtlasScale = 1.0f;

    // 아틀라스 창을 메인 창이 있는 모니터의 작업 영역 안, 메인 창 오른쪽에 둔다.
    // 자리가 모자라면 화면 오른쪽 끝 안쪽(메인 창 위)에 겹쳐 둔다. 전에는 x=1600에 고정이라 작은 화면에서 밖으로 나갔다
    static bool ToolScenePlaceAtlasWindow(HWND atlas, HWND owner)
    {
        RECT ownerRect = {};
        RECT atlasRect = {};
        if (!GetWindowRect(owner, &ownerRect) || !GetWindowRect(atlas, &atlasRect))
            return false;

        MONITORINFO info = {};
        info.cbSize = sizeof(MONITORINFO);
        if (!GetMonitorInfo(MonitorFromWindow(owner, MONITOR_DEFAULTTONEAREST), &info))
            return false;

        const RECT& work = info.rcWork;
        int width = atlasRect.right - atlasRect.left;
        int height = atlasRect.bottom - atlasRect.top;

        // 작업 영역에 다 들어가도록 배율을 정하고 창 크기를 맞춘다 (작업 영역 높이 852px인 화면에서는 아래쪽 타일이 잘려 고를 수 없었다)
        Image* atlasImage = Resources::Find<Image>(L"TileAtlas");
        if (atlasImage != nullptr && atlasImage->GetWidth() > 0 && atlasImage->GetHeight() > 0)
        {
            RECT frame = { 0, 0, 0, 0 };
            AdjustWindowRect(&frame, WS_OVERLAPPEDWINDOW, false);
            const int frameWidth = frame.right - frame.left;
            const int frameHeight = frame.bottom - frame.top;

            float scale = 1.0f;
            float scaleX = (float)(work.right - work.left - frameWidth) / atlasImage->GetWidth();
            float scaleY = (float)(work.bottom - work.top - frameHeight) / atlasImage->GetHeight();
            if (scaleX < scale)
                scale = scaleX;
            if (scaleY < scale)
                scale = scaleY;
            ToolSceneAtlasScale = scale;

            width = (int)(atlasImage->GetWidth() * scale) + frameWidth;
            height = (int)(atlasImage->GetHeight() * scale) + frameHeight;
        }

        int x = ownerRect.right;
        if (x + width > work.right)
            x = work.right - width;
        if (x < work.left)
            x = work.left;

        int y = (ownerRect.top > work.top) ? ownerRect.top : work.top;
        if (y + height > work.bottom)
            y = work.bottom - height;
        if (y < work.top)
            y = work.top;

        SetWindowPos(atlas, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(atlas, nullptr, TRUE);

        Scenario::Publish("tool.atlasScale", ToolSceneAtlasScale);
        Scenario::Publish("tool.atlasX", x);
        Scenario::Publish("tool.atlasY", y);
        Scenario::Publish("tool.atlasW", width);
        Scenario::Publish("tool.atlasH", height);
        Scenario::Publish("tool.workW", work.right - work.left);
        Scenario::Publish("tool.workH", work.bottom - work.top);

        // 창 전체가 작업 영역 안에 들어갔는가 (창이 작업 영역보다 크면 false)
        return x >= work.left && y >= work.top && x + width <= work.right && y + height <= work.bottom;
    }

    ToolScene::ToolScene()
        : mbPainting(false)
        , mLastPaintCell(Vector2::Zero)
    {
    }
    ToolScene::~ToolScene()
    {
    }
    void ToolScene::Initialize()
    {

        Scene::Initialize();

        TilePalatte::Intialize();

        // 시나리오 검사용 초깃값 (tool.tiles: 칸별 타일 수, tool.atlasOnScreen: 아틀라스 창이 화면 안에 있는가)
        Scenario::Publish("tool.tiles", 0.0);
        Scenario::Publish("tool.atlasOnScreen", 0.0);
    }
    void ToolScene::Update()
    {
        Scene::Update();

        Vector2  temp = Input::GetMousePos();


        // 창 밖 클릭 무시. CreateTile은 씬의 타일 로드에도 쓰이므로 검사는 여기서만 한다.
        Vector2 mousePos = Input::GetMousePos();
        bool mouseInWindow = mousePos.x > 0.0f && mousePos.x < 1600.0f
            && mousePos.y > 0.0f && mousePos.y < 900.0f;

        // 누르는 순간과, 누른 채 다른 칸으로 옮겼을 때만 칠한다.
        // 전에는 누르고 있는 프레임마다 같은 칸에 타일이 새로 쌓였고, 목록에는 첫 장만 남아 우클릭으로 지워지지 않았다
        bool leftHeld = Input::GetKeyDown(eKeyCode::LBUTTON) || Input::GetKey(eKeyCode::LBUTTON);
        if (!leftHeld)
            mbPainting = false;

        if (mouseInWindow && leftHeld)
        {
            Vector2 pos = Input::GetMousePos();
            pos -= Camera::CaluatePos(Vector2::Zero);

            pos = TilePalatte::GetTilePos(pos);

            if (!mbPainting || pos.x != mLastPaintCell.x || pos.y != mLastPaintCell.y)
            {
                UINT tileIndex = TilePalatte::GetIndex();
                TilePalatte::PaintTile(tileIndex, pos);
                mLastPaintCell = pos;
                mbPainting = true;
            }
        }

        if (Input::GetKeyDown(eKeyCode::RBUTTON))
        {
            Vector2 pos = Input::GetMousePos();
            pos -= Camera::CaluatePos(Vector2::Zero);

            pos = TilePalatte::GetTilePos(pos);

            TilePalatte::DeleateTile(pos);

        }
        
        if (Input::GetKeyDown(eKeyCode::S))
        {
            TilePalatte::Save();
        }
        if (Input::GetKeyDown(eKeyCode::L))
        {
            TilePalatte::Load();
        }

		if (Input::GetKeyState(eKeyCode::N) == eKeyState::Down)
		{
			SceneManager::LoadScene(eSceneType::Title);
		}

        Scenario::Publish("tool.tiles", (double)TilePalatte::GetTileCount());
    }

    void ToolScene::Render(HDC hdc)
    {
        // 타일 격자 (흰색 2px 선)
        const COLORREF gridColor = RGB(255, 255, 255);
        const int gridThickness = 2;

        Vector2 startPos(0, 0);
        startPos = Camera::CaluatePos(startPos);

        int maxRow = application.GetHeight() / TILE_SIZE_Y + 1;
        for (size_t y = 0; y < maxRow * 3; y++)
        {
            Renderer::DrawLine((int)startPos.x, (int)(TILE_SIZE_Y * y + startPos.y)
                , (int)application.GetWidth(), (int)(TILE_SIZE_Y * y + startPos.y)
                , gridColor, gridThickness);
        }
        int maxColumn = application.GetWidth() / TILE_SIZE_X + 1;
        for (size_t x = 0; x < maxColumn * 3; x++)
        {
            Renderer::DrawLine((int)(TILE_SIZE_X * x + startPos.x), (int)startPos.y
                , (int)(TILE_SIZE_X * x + startPos.x), (int)application.GetHeight()
                , gridColor, gridThickness);
        }

        Scene::Render(hdc);
    }
    void ToolScene::Release()
    {
        Scene::Release();
    }
    void ToolScene::OnEnter()
    {
        // 칸별 타일 목록을 이 씬의 타일로만 다시 만든다.
        // 목록은 씬 사이에 공유돼서, 스테이지를 다녀오면 스테이지 타일이 섞여 저장·삭제가 엉뚱한 타일을 건드렸다
        TilePalatte::BindToScene(this);
        mbPainting = false;

        // 아틀라스 창은 타일 편집 중에만 보여준다. 메인 창의 소유 창이라 메인 창보다 항상 위에 있다 (main.cpp)
        HWND toolHwnd = application.GetToolHwnd();
        bool onScreen = ToolScenePlaceAtlasWindow(toolHwnd, application.GetHwnd());
        Scenario::Publish("tool.atlasOnScreen", onScreen ? 1.0 : 0.0);
        if (!application.IsHeadless())
            ShowWindow(toolHwnd, SW_SHOWNOACTIVATE);
    }
    void ToolScene::OnExit()
    {
        ShowWindow(application.GetToolHwnd(), SW_HIDE);
    }
}

LRESULT CALLBACK AtlasWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {


    case WM_CREATE:
    {
        ya::Image* tile = ya::Resources::Load<ya::Image>(L"TileAtlas", L"..\\Resources\\SkulAtlas2x2.bmp");
        RECT rect = { 0, 0, tile->GetWidth(), tile->GetHeight() };
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

        // 크기만 맞춘다. 위치는 툴 씬에 들어갈 때 모니터 안쪽으로 정한다 (ToolScene::OnEnter)
        SetWindowPos(hWnd
            , nullptr, 0, 0
            , rect.right - rect.left
            , rect.bottom - rect.top
            , SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    break; // 전에는 break가 없어 생성 중에 아래 클릭 처리(타일 선택)까지 실행됐다

    case WM_LBUTTONDOWN:
    {
        // 클릭 위치(클라이언트 좌표)를 그리는 배율로 되돌려 아틀라스 칸을 고른다.
        // 전에는 창이 포커스를 가져야만(GetFocus) 골랐고, 배율 없이 계산했다
        ya::Image* atlas = ya::Resources::Find<ya::Image>(L"TileAtlas");
        const float scale = ya::ToolSceneAtlasScale;
        const int columns = (atlas != nullptr) ? (int)atlas->GetWidth() / TILE_SIZE_X : 14;

        int x = (int)((short)LOWORD(lParam) / scale) / TILE_SIZE_X;
        int y = (int)((short)HIWORD(lParam) / scale) / TILE_SIZE_Y;
        if (x >= 0 && y >= 0 && x < columns)
        {
            ya::TilePalatte::SetIndex(y * columns + x);
            InvalidateRect(hWnd, nullptr, FALSE); // 선택 테두리를 다시 그린다
        }
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
            //case IDM_ABOUT:
            //    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            //    break;
            //case IDM_EXIT:
            //    DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        ya::Image* tile = ya::Resources::Find<ya::Image>(L"TileAtlas");
        const float scale = ya::ToolSceneAtlasScale;

        // 화면에 맞춘 배율로 그린다 (픽셀 아트라 보간 없이)
        SetStretchBltMode(hdc, COLORONCOLOR);
        StretchBlt(hdc, 0, 0, (int)(tile->GetWidth() * scale), (int)(tile->GetHeight() * scale)
            , tile->GetHdc(), 0, 0, tile->GetWidth(), tile->GetHeight(), SRCCOPY);

        // 지금 고른 타일에 붉은 테두리
        const int columns = (int)tile->GetWidth() / TILE_SIZE_X;
        const int index = (int)ya::TilePalatte::GetIndex();
        if (columns > 0 && index >= 0)
        {
            const int cellX = index % columns;
            const int cellY = index / columns;
            RECT cell = { (LONG)(cellX * TILE_SIZE_X * scale), (LONG)(cellY * TILE_SIZE_Y * scale)
                , (LONG)((cellX + 1) * TILE_SIZE_X * scale), (LONG)((cellY + 1) * TILE_SIZE_Y * scale) };
            HBRUSH frameBrush = CreateSolidBrush(RGB(255, 60, 60));
            FrameRect(hdc, &cell, frameBrush);
            InflateRect(&cell, -1, -1);
            FrameRect(hdc, &cell, frameBrush);
            DeleteObject(frameBrush);
        }
        //Ellipse(hdc, 500, 500, 600, 700);
        //RoundRect(hdc, 200, 200, 300, 300, 500, 500);

        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;


    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}