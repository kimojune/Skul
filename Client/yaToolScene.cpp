#include "yaToolScene.h"
#include "yaApplication.h"
#include "yaImage.h"
#include "yaResources.h"
#include "yaTile.h"
#include "yaObject.h"
#include "yaInput.h"
#include "yaTilePalatte.h"
#include "yaCamera.h"

extern ya::Application application;

namespace ya
{
    ToolScene::ToolScene()
    {
    }
    ToolScene::~ToolScene()
    {
    }
    void ToolScene::Initialize()
    {

        Scene::Initialize();

        TilePalatte::Intialize();

    }
    void ToolScene::Update()
    {
        Scene::Update();

        Vector2  temp = Input::GetMousePos();


        // 창 밖 클릭 무시. CreateTile은 씬의 타일 로드에도 쓰이므로 검사는 여기서만 한다.
        Vector2 mousePos = Input::GetMousePos();
        bool mouseInWindow = mousePos.x > 0.0f && mousePos.x < 1600.0f
            && mousePos.y > 0.0f && mousePos.y < 900.0f;

        if (mouseInWindow && Input::GetKey(eKeyCode::LBUTTON))
        {
            Vector2 pos = Input::GetMousePos();
            pos -= Camera::CaluatePos(Vector2::Zero);

            pos = TilePalatte::GetTilePos(pos);

            UINT tileIndex = TilePalatte::GetIndex();
            TilePalatte::CreateTile(tileIndex, pos);
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
        // 아틀라스 창은 타일 편집 중에만 보여준다.
        ShowWindow(application.GetToolHwnd(), SW_SHOWNOACTIVATE);
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

        SetWindowPos(hWnd
            , nullptr, 1600, 0
            , rect.right - rect.left
            , rect.bottom - rect.top
            , 0);
    }

    case WM_LBUTTONDOWN:
    {

        if (GetFocus())
        {
            ::POINT mousePos = {};
            ::GetCursorPos(&mousePos);
            ::ScreenToClient(application.GetToolHwnd(), &mousePos);

            int x = mousePos.x / TILE_SIZE_X;
            int y = mousePos.y / TILE_SIZE_Y;

            int index = (y * 14) + (x % 14);

            ya::TilePalatte::SetIndex(index);
        }
    }

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
        BitBlt(hdc, 0, 0, tile->GetWidth(), tile->GetHeight(), tile->GetHdc(), 0, 0, SRCCOPY);
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