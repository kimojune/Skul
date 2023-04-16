#include "yaTilePalatte.h"
#include "yaResources.h"
#include "yaInput.h"
#include <commdlg.h>

namespace ya
{
	Image* TilePalatte::mImage = nullptr;
	std::unordered_map<UINT64, Tile*> TilePalatte::mTiles = {};
	UINT TilePalatte::mIndex = -1;

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
		Vector2 mousPos = Input::GetMousePos();
		if (mousPos.x >= 1600.0f || mousPos.x <= 0.0f)
			return;
		if (mousPos.y >= 900.0f || mousPos.y <= 0.0f)
			return;

		Tile* tile = object::Instantiate<Tile>(eLayerType::Tile);
		tile->InitializeTile(mImage, index);

		Vector2 tilePos(pos.x * TILE_SIZE_X, pos.y * TILE_SIZE_Y);
		//Image::Create(L"TilePixel", TILE_SIZE_X, TILE_SIZE_Y, RGB(255, 0, 255));

		tile->GetComponent<Transform>()->SetPos(tilePos);

		TileID id;
		id.x = (UINT32)pos.x;
		id.y = (UINT32)pos.y;

		mTiles.insert(std::make_pair(id.id, tile));
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


		mTiles.erase(id.id);

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

		// ���� �����
		FILE* file = nullptr;

		// ���� ���� ����
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
	}

	void TilePalatte::Load(const std::wstring& path)
	{

		// LPWSTR ���� ���� �� �ʱ�ȭ
		LPWSTR lpwstr = new WCHAR[path.size() + 1];

		// std::wstring ������ LPWSTR�� ����
		wcscpy_s(lpwstr, path.size() + 1, path.c_str());

		FILE* file = nullptr;
		_wfopen_s(&file, lpwstr, L"rb");

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

		delete[] lpwstr;
	}


	Vector2 TilePalatte::GetTilePos(Vector2 mousePos)
	{
		int indexY = mousePos.y / TILE_SIZE_Y;
		int indexX = mousePos.x / TILE_SIZE_X;

		return Vector2(indexX, indexY);
	}
}