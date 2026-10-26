#pragma once
#include "yaTile.h"
#include "yaImage.h"
#include "yaObject.h"

namespace ya
{
	union TileID
	{
		struct
		{
			UINT32 x;
			UINT32 y;
		};
		UINT64 id;
	};

class TilePalatte
{
public:
	static void Intialize();
	static void Update();
	static void Render(HDC hdc);

	static void CreateTile(int index, Vector2 pos);
	static void DeleateTile (Vector2 pos);
	// 툴에서 칠하기: 그 칸에 타일이 있으면 새 그림으로 바꾸고, 없으면 만든다 (한 칸에 한 장)
	static void PaintTile(int index, Vector2 pos);
	// 칸별 타일 목록을 이 씬의 Tile 레이어로 다시 만든다. 목록은 씬 사이에 공유되므로 툴에 들어올 때 부른다
	static void BindToScene(Scene* scene);
	static size_t GetTileCount() { return mTiles.size(); }
	static void CreateTiles(int index, UINT width, UINT height);
	static void Save();
	static void Load();
	static void Load(const std::wstring& path);

	static Vector2 GetTilePos(Vector2 mousePos);
	static void SetIndex(UINT index) { mIndex = index; }
	static UINT GetIndex() { return mIndex; }


private:
	static std::unordered_map<UINT64, Tile*>mTiles;
	static Image* mImage;
	static UINT mIndex;

	
};

}
