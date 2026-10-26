#include "yaTilePalatte.h"
#include "yaResources.h"
#include "yaInput.h"
#include "yaCollisionMap.h"
#include "yaSceneManager.h"
#include <commdlg.h>

namespace ya
{
	
	Image* TilePalatte::mImage = nullptr;
	std::unordered_map<UINT64, Tile*> TilePalatte::mTiles = {};
	UINT TilePalatte::mIndex = -1;

	UINT32 mMaxX;
	UINT32 mMaxY;

	void TilePalatte::Intialize()
	{
		mImage = Resources::Find<Image>(L"TileAtlas");
	}
	void TilePalatte::Update()
	{
	}
	void TilePalatte::Render(HDC hdc)
	{
	}
	void TilePalatte::CreateTile(int index, Vector2 pos)
	{
		TileID id;
		id.x = (UINT32)pos.x;
		id.y = (UINT32)pos.y;


		std::unordered_map<UINT64, Tile*>::iterator iter = mTiles.find(id.id);

		//if (iter == mTiles.end())
		//{
			Tile* tile = object::Instantiate<Tile>(eLayerType::Tile);
			tile->InitializeTile(mImage, index);

			Vector2 tilePos(pos.x * TILE_SIZE_X, pos.y * TILE_SIZE_Y);
			tile->GetComponent<Transform>()->SetPos(tilePos);

			mTiles.insert(std::make_pair(id.id, tile));
		//}
		//else
		//	return;

	}


	void TilePalatte::DeleateTile(Vector2 pos)
	{

		Vector2 mousPos = Input::GetMousePos();
		if (mousPos.x >= 1600.0f || mousPos.x <= 0.0f)
			return;
		if (mousPos.y >= 900.0f || mousPos.y <= 0.0f)
			return;

		TileID id;
		id.x = (UINT32)pos.x;
		id.y = (UINT32)pos.y;
		
		std::unordered_map<UINT64, Tile*>::iterator iter = mTiles.find(id.id);
		
		if (iter == mTiles.end())
			return;

		Tile* tile = iter->second;

		tile->SetState(GameObject::eState::Death);
		mTiles.erase(id.id);

	}

	void TilePalatte::PaintTile(int index, Vector2 pos)
	{
		TileID id;
		id.x = (UINT32)pos.x;
		id.y = (UINT32)pos.y;

		std::unordered_map<UINT64, Tile*>::iterator iter = mTiles.find(id.id);
		if (iter != mTiles.end())
		{
			if ((int)iter->second->Index() == index)
				return;

			// 다른 그림이면 기존 타일을 지우고 새로 만든다 (InitializeTile을 다시 부르면 컴포넌트까지 다시 초기화된다)
			iter->second->SetState(GameObject::eState::Death);
			mTiles.erase(iter);
		}

		CreateTile(index, pos);
	}

	void TilePalatte::BindToScene(Scene* scene)
	{
		mTiles.clear();
		if (scene == nullptr)
			return;

		// 같은 칸에 여러 장이 있으면 나중 것(위에 그려지는 것)을 목록에 둔다
		for (GameObject* gameObj : scene->GetGameObjects(eLayerType::Tile))
		{
			Tile* tile = dynamic_cast<Tile*>(gameObj);
			if (tile == nullptr || tile->GetState() == GameObject::eState::Death)
				continue;

			Vector2 tilePos = tile->GetComponent<Transform>()->GetPos();
			TileID id;
			id.x = (UINT32)(tilePos.x / TILE_SIZE_X);
			id.y = (UINT32)(tilePos.y / TILE_SIZE_Y);
			mTiles[id.id] = tile;
		}
	}

	void TilePalatte::CreateTiles(int index, UINT width, UINT height)
	{
		
	}
	void TilePalatte::Save()
	{
		OPENFILENAME ofn = {};

		wchar_t szFilePath[256] = {};

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFilePath;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = 256;
		ofn.lpstrFilter = L"Tile\0*.tile\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (false == GetSaveFileName(&ofn))
			return;

		// 파일 입출력
		FILE* file = nullptr;

		// 쓰기 모드로 열기
		_wfopen_s(&file, szFilePath, L"wb");

		if (nullptr == file)
			return;

		std::unordered_map<UINT64, Tile*>::iterator iter = mTiles.begin();
		for (; iter != mTiles.end(); iter++)
		{
			int index = iter->second->Index();
		
			if (mIndex == -1)
			{
				continue;
			}
			
			fwrite(&index, sizeof(int), 1, file);

			TileID id;
			id.id = iter->first;
			fwrite(&id.id, sizeof(TileID), 1, file);
		}

		fclose(file);
	}
	void TilePalatte::Load()
	{
		OPENFILENAME ofn = {};

		wchar_t szFilePath[256] = {};

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFilePath;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = 256;
		ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (false == GetOpenFileName(&ofn))
			return;

		FILE* file = nullptr;
		_wfopen_s(&file, szFilePath, L"rb");

		if (file == nullptr)
			return;

		while (true)
		{
			int index = -1;
			TileID id;

			if (fread(&index, sizeof(int), 1, file) == NULL)
				break;

			if (fread(&id.id, sizeof(TileID), 1, file) == NULL)
				break;

			CreateTile(index, Vector2(id.x, id.y));
		}

		fclose(file);

		CollisionMap::MarkDirty(SceneManager::GetActiveScene());
	}

	void TilePalatte::Load(const std::wstring& path)
	{

		// LPWSTR 변수 선언 및 초기화
		LPWSTR lpwstr = new WCHAR[path.size() + 1];

		// std::wstring 내용을 LPWSTR로 복사
		wcscpy_s(lpwstr, path.size() + 1, path.c_str());

		

		FILE* file = nullptr;
		_wfopen_s(&file, lpwstr, L"rb");
		delete[] lpwstr;

		if (file == nullptr)
			return;

		while (true)
		{
			int index = -1;
			TileID id;

			if (fread(&index, sizeof(int), 1, file) == NULL)
				break;

			if (fread(&id.id, sizeof(TileID), 1, file) == NULL)
				break;

			CreateTile(index, Vector2(id.x, id.y));
		}

		fclose(file);

		// 타일이 바뀌었으니 지금 씬의 픽셀 충돌 맵을 다시 만든다 (보이는 타일 픽셀 = 땅).
		// 실제로 만드는 것은 처음 쓸 때 한 번 (Stage2처럼 파일을 둘 불러와도 한 번만 만든다)
		CollisionMap::MarkDirty(SceneManager::GetActiveScene());
	}


	Vector2 TilePalatte::GetTilePos(Vector2 mousePos)
	{
		int indexY = mousePos.y / TILE_SIZE_Y;
		int indexX = mousePos.x / TILE_SIZE_X;

		return Vector2(indexX, indexY);
	}
}