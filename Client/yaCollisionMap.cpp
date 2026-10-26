#include "yaCollisionMap.h"
#include "yaScene.h"
#include "yaSceneManager.h"
#include "yaGameObject.h"
#include "yaTransform.h"
#include "yaCollider.h"
#include "yaTile.h"
#include "yaPlatform.h"
#include "yaImage.h"

namespace ya
{
	std::vector<unsigned char> CollisionMap::mPixels = {};
	int CollisionMap::mWidth = 0;
	int CollisionMap::mHeight = 0;
	Scene* CollisionMap::mScene = nullptr;
	Scene* CollisionMap::mDirtyScene = nullptr;

	// 아틀라스 이미지의 불투명 마스크 (1 = 마젠타 키가 아님). 아틀라스마다 한 번만 읽는다.
	struct CollisionMapAtlasMask
	{
		int width = 0;
		int height = 0;
		std::vector<unsigned char> opaque;
	};

	static const CollisionMapAtlasMask& CollisionMapGetAtlasMask(Image* atlas)
	{
		static std::map<Image*, CollisionMapAtlasMask> cache;

		std::map<Image*, CollisionMapAtlasMask>::iterator iter = cache.find(atlas);
		if (iter != cache.end())
			return iter->second;

		CollisionMapAtlasMask& mask = cache[atlas];
		mask.width = (int)atlas->GetWidth();
		mask.height = (int)atlas->GetHeight();
		mask.opaque.assign((size_t)mask.width * mask.height, 0);

		// 빠른 길: LoadImage(LR_CREATEDIBSECTION)로 읽은 24/32비트 DIB는 픽셀 메모리를 바로 읽는다
		DIBSECTION dib = {};
		HBITMAP bitmap = atlas->GetBitmap();
		if (bitmap != NULL
			&& GetObject(bitmap, sizeof(DIBSECTION), &dib) == sizeof(DIBSECTION)
			&& dib.dsBm.bmBits != nullptr
			&& (dib.dsBm.bmBitsPixel == 24 || dib.dsBm.bmBitsPixel == 32)
			&& dib.dsBm.bmWidth == mask.width
			&& dib.dsBm.bmHeight == mask.height)
		{
			GdiFlush();
			const BYTE* bits = (const BYTE*)dib.dsBm.bmBits;
			const int bytesPerPixel = dib.dsBm.bmBitsPixel / 8;
			const int stride = ((mask.width * dib.dsBm.bmBitsPixel + 31) / 32) * 4;
			const bool bottomUp = dib.dsBmih.biHeight > 0;

			for (int y = 0; y < mask.height; y++)
			{
				const BYTE* row = bits + (size_t)(bottomUp ? (mask.height - 1 - y) : y) * stride;
				for (int x = 0; x < mask.width; x++)
				{
					const BYTE* pixel = row + x * bytesPerPixel;
					const bool magenta = (pixel[2] == 255 && pixel[1] == 0 && pixel[0] == 255); // BGR
					mask.opaque[(size_t)y * mask.width + x] = magenta ? 0 : 1;
				}
			}
			return mask;
		}

		// 느린 길: Image::GetPixel (아틀라스당 한 번)
		for (int y = 0; y < mask.height; y++)
		{
			for (int x = 0; x < mask.width; x++)
			{
				COLORREF color = atlas->GetPixel(x, y);
				const bool transparent = (color == RGB(255, 0, 255) || color == CLR_INVALID);
				mask.opaque[(size_t)y * mask.width + x] = transparent ? 0 : 1;
			}
		}
		return mask;
	}

	// 타일 한 칸을 Tile::Render와 같은 원본 영역으로 칠한다. 불투명 픽셀이 하나라도 있었으면 true
	static bool CollisionMapPaintTile(std::vector<unsigned char>& pixels, int mapWidth, int mapHeight
		, Tile* tile, CollisionMap::ePixel value)
	{
		Image* atlas = tile->GetAtlas();
		if (atlas == nullptr)
			return false;

		const CollisionMapAtlasMask& mask = CollisionMapGetAtlasMask(atlas);
		Vector2 pos = tile->GetComponent<Transform>()->GetPos();
		const int dstX = (int)pos.x;
		const int dstY = (int)pos.y;
		const int srcX = (int)tile->X() * TILE_SIZE_X;
		const int srcY = (int)tile->Y() * TILE_SIZE_Y;

		bool painted = false;
		for (int j = 0; j < TILE_SIZE_Y; j++)
		{
			const int sy = srcY + j;
			const int dy = dstY + j;
			if (sy < 0 || sy >= mask.height || dy < 0 || dy >= mapHeight)
				continue;

			for (int i = 0; i < TILE_SIZE_X; i++)
			{
				const int sx = srcX + i;
				const int dx = dstX + i;
				if (sx < 0 || sx >= mask.width || dx < 0 || dx >= mapWidth)
					continue;

				if (mask.opaque[(size_t)sy * mask.width + sx] == 0)
					continue;

				pixels[(size_t)dy * mapWidth + dx] = (unsigned char)value;
				painted = true;
			}
		}
		return painted;
	}

	// 맵에 칠할 타일만 고른다 (지워진 타일, 아틀라스 없는 타일 제외)
	static Tile* CollisionMapAsTile(GameObject* obj)
	{
		Tile* tile = dynamic_cast<Tile*>(obj);
		if (tile == nullptr || tile->GetState() == GameObject::eState::Death || tile->GetAtlas() == nullptr)
			return nullptr;

		return tile;
	}

	// Platform 사각형 (월드 좌표). 콜라이더가 없으면 false
	static bool CollisionMapPlatformRect(Platform* platform, Vector2& leftTop, Vector2& size)
	{
		Collider* collider = platform->GetComponent<Collider>();
		if (collider == nullptr)
			return false;

		collider->Update(); // 콜라이더 왼쪽 위 = 위치 + center
		leftTop = collider->GetPos();
		size = collider->GetSize();
		return true;
	}

	void CollisionMap::Rebuild(Scene* scene)
	{
		Clear();
		if (scene == nullptr)
			return;

		mScene = scene;

		std::vector<GameObject*>& tileObjects = scene->GetGameObjects(eLayerType::Tile);
		std::vector<GameObject*>& groundObjects = scene->GetGameObjects(eLayerType::Ground);

		std::vector<Platform*> platforms = {};
		for (GameObject* obj : groundObjects)
		{
			Platform* platform = dynamic_cast<Platform*>(obj);
			if (platform != nullptr && platform->GetState() == GameObject::eState::Active)
				platforms.push_back(platform);
		}

		// 1) 맵 크기 = 타일과 발판을 모두 덮는 크기 (원점은 월드 0,0)
		int width = 0;
		int height = 0;
		for (GameObject* obj : tileObjects)
		{
			Tile* tile = CollisionMapAsTile(obj);
			if (tile == nullptr)
				continue;

			Vector2 pos = tile->GetComponent<Transform>()->GetPos();
			width = max(width, (int)pos.x + TILE_SIZE_X);
			height = max(height, (int)pos.y + TILE_SIZE_Y);
		}
		for (Platform* platform : platforms)
		{
			Vector2 leftTop, size;
			if (!CollisionMapPlatformRect(platform, leftTop, size))
				continue;

			width = max(width, (int)ceilf(leftTop.x + size.x));
			height = max(height, (int)ceilf(leftTop.y + size.y));
		}

		if (width <= 0 || height <= 0)
			return;

		mWidth = width;
		mHeight = height;
		mPixels.assign((size_t)mWidth * mHeight, (unsigned char)ePixel::Empty);

		// 2) 보이는 타일 픽셀 = 단단한 땅
		for (GameObject* obj : tileObjects)
		{
			Tile* tile = CollisionMapAsTile(obj);
			if (tile != nullptr)
				CollisionMapPaintTile(mPixels, mWidth, mHeight, tile, ePixel::Solid);
		}

		// 3) 발판: Platform 사각형에 걸친 타일의 보이는 픽셀을 OneWay로 덮어쓴다.
		//    사각형만 칠하면 발판 그림의 윗부분(사각형보다 몇 픽셀 위)이 Solid로 남아 내려갈 수 없게 된다.
		for (Platform* platform : platforms)
		{
			Vector2 leftTop, size;
			if (!CollisionMapPlatformRect(platform, leftTop, size))
				continue;

			bool painted = false;
			for (GameObject* obj : tileObjects)
			{
				Tile* tile = CollisionMapAsTile(obj);
				if (tile == nullptr)
					continue;

				Vector2 pos = tile->GetComponent<Transform>()->GetPos();
				const bool overlap = pos.x < leftTop.x + size.x && leftTop.x < pos.x + TILE_SIZE_X
					&& pos.y < leftTop.y + size.y && leftTop.y < pos.y + TILE_SIZE_Y;
				if (!overlap)
					continue;

				if (CollisionMapPaintTile(mPixels, mWidth, mHeight, tile, ePixel::OneWay))
					painted = true;
			}

			if (painted)
				continue;

			// 그림이 없는 발판(보스 스테이지): 사각형 전체
			const int left = max(0, (int)leftTop.x);
			const int top = max(0, (int)leftTop.y);
			const int right = min(mWidth, (int)ceilf(leftTop.x + size.x));
			const int bottom = min(mHeight, (int)ceilf(leftTop.y + size.y));
			for (int y = top; y < bottom; y++)
			{
				for (int x = left; x < right; x++)
					mPixels[(size_t)y * mWidth + x] = (unsigned char)ePixel::OneWay;
			}
		}

		// 디버그: SKUL_COLLISION_BMP=<폴더> 이면 맵을 BMP로 저장
		wchar_t folder[MAX_PATH] = {};
		DWORD length = GetEnvironmentVariableW(L"SKUL_COLLISION_BMP", folder, MAX_PATH);
		if (length > 0 && length < MAX_PATH)
		{
			std::wstring path = std::wstring(folder) + L"\\collision_"
				+ std::to_wstring((int)SceneManager::GetActiveSceneType()) + L".bmp";
			SaveBmp(path);
		}
	}

	void CollisionMap::Clear()
	{
		mPixels.clear();
		mWidth = 0;
		mHeight = 0;
		mScene = nullptr;
	}

	bool CollisionMap::IsReady()
	{
		Scene* activeScene = SceneManager::GetActiveScene();
		if (mDirtyScene != nullptr && mDirtyScene == activeScene)
		{
			mDirtyScene = nullptr;
			Rebuild(activeScene);
		}

		return mScene != nullptr && mScene == activeScene;
	}

	CollisionMap::ePixel CollisionMap::Get(int x, int y)
	{
		if (mWidth <= 0 || y < 0 || y >= mHeight)
			return ePixel::Empty;

		if (x < 0)
			x = 0;
		else if (x >= mWidth)
			x = mWidth - 1;

		return (ePixel)mPixels[(size_t)y * mWidth + x];
	}

	bool CollisionMap::IsSolidRow(int left, int right, int row)
	{
		for (int x = left; x < right; x++)
		{
			if (Get(x, row) == ePixel::Solid)
				return true;
		}
		return false;
	}

	// 윗면 위로 이만큼은 Solid가 없어야 설 수 있는 곳으로 본다.
	// 벽 가장자리 그림의 오목한 틈(몇 픽셀)을 윗면으로 잡아 벽을 타고 오르지 않게 한다.
	static const int CollisionMapClearance = 16;

	static bool CollisionMapHasClearance(int x, int row)
	{
		for (int k = 1; k <= CollisionMapClearance; k++)
		{
			if (CollisionMap::Get(x, row - k) == CollisionMap::ePixel::Solid)
				return false;
		}
		return true;
	}

	int CollisionMap::FindSurface(int left, int right, int fromRow, int toRow, int oneWayFrom, ePixel* type)
	{
		for (int row = fromRow; row <= toRow; row++)
		{
			bool oneWay = false;
			for (int x = left; x < right; x++)
			{
				ePixel pixel = Get(x, row);
				if (pixel == ePixel::Empty)
					continue;

				ePixel above = Get(x, row - 1);
				if (pixel == ePixel::Solid && above != ePixel::Solid && CollisionMapHasClearance(x, row))
				{
					if (type != nullptr)
						*type = ePixel::Solid;
					return row;
				}

				if (pixel == ePixel::OneWay && above == ePixel::Empty && row >= oneWayFrom
					&& CollisionMapHasClearance(x, row))
					oneWay = true;
			}

			if (oneWay)
			{
				if (type != nullptr)
					*type = ePixel::OneWay;
				return row;
			}
		}
		return NoSurface;
	}

	int CollisionMap::FindSolidSurfaceAbove(int left, int right, int fromRow, int toRow)
	{
		for (int row = fromRow; row >= toRow; row--)
		{
			for (int x = left; x < right; x++)
			{
				if (Get(x, row) == ePixel::Solid && Get(x, row - 1) != ePixel::Solid
					&& CollisionMapHasClearance(x, row))
					return row;
			}
		}
		return NoSurface;
	}

	bool CollisionMap::SaveBmp(const std::wstring& path)
	{
		if (mWidth <= 0 || mHeight <= 0)
			return false;

		const int stride = (mWidth + 3) & ~3;
		const DWORD paletteSize = 256 * sizeof(RGBQUAD);
		const DWORD imageSize = (DWORD)stride * mHeight;

		BITMAPFILEHEADER fileHeader = {};
		fileHeader.bfType = 0x4D42; // "BM"
		fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize;
		fileHeader.bfSize = fileHeader.bfOffBits + imageSize;

		BITMAPINFOHEADER infoHeader = {};
		infoHeader.biSize = sizeof(BITMAPINFOHEADER);
		infoHeader.biWidth = mWidth;
		infoHeader.biHeight = mHeight; // 아래 줄부터 저장
		infoHeader.biPlanes = 1;
		infoHeader.biBitCount = 8;
		infoHeader.biCompression = BI_RGB;
		infoHeader.biSizeImage = imageSize;
		infoHeader.biClrUsed = 256;

		RGBQUAD palette[256] = {};
		palette[(int)ePixel::Solid] = { 255, 0, 255, 0 };   // B, G, R: 마젠타
		palette[(int)ePixel::OneWay] = { 0, 255, 255, 0 };  // 노랑

		FILE* file = nullptr;
		_wfopen_s(&file, path.c_str(), L"wb");
		if (file == nullptr)
			return false;

		fwrite(&fileHeader, sizeof(fileHeader), 1, file);
		fwrite(&infoHeader, sizeof(infoHeader), 1, file);
		fwrite(palette, sizeof(palette), 1, file);

		std::vector<unsigned char> row((size_t)stride, 0);
		for (int y = mHeight - 1; y >= 0; y--)
		{
			memcpy(row.data(), &mPixels[(size_t)y * mWidth], (size_t)mWidth);
			fwrite(row.data(), 1, row.size(), file);
		}

		fclose(file);
		return true;
	}
}
