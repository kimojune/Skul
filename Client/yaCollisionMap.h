#pragma once
#include "YamYamEngine.h"

namespace ya
{
	class Scene;
	class Image;

	// 타일 그림에서 만든 픽셀 단위 지형 충돌 맵. 월드와 같은 크기, 1픽셀 = 1바이트.
	//  - Solid  : 보이는 타일 픽셀(마젠타 키가 아닌 픽셀) = 단단한 땅. BMP로 저장하면 마젠타(255,0,255)
	//  - OneWay : Platform 오브젝트가 걸친 타일의 보이는 픽셀 = 아래에서 통과하는 발판. BMP에서는 노랑(255,255,0)
	//             걸친 타일 그림이 없는 발판(보스 스테이지)은 Platform 사각형 전체를 OneWay로 칠한다.
	// 스테이지 OnEnter에서 타일을 불러올 때(TilePalatte::Load) 그 씬 기준으로 다시 만든다.
	// 착지 판정은 Rigidbody가 이 맵으로 한다 (yaRigidbody.cpp).
	class CollisionMap
	{
	public:
		enum class ePixel : unsigned char
		{
			Empty,
			Solid,
			OneWay,
		};

		static const int NoSurface = INT_MAX;

		// 씬의 Tile 레이어(타일)와 Ground 레이어(Platform)로 맵을 다시 만든다.
		// 환경 변수 SKUL_COLLISION_BMP에 폴더를 주면 그 폴더에 collision_<씬 번호>.bmp로 저장한다.
		static void Rebuild(Scene* scene);
		// 타일이 바뀌었다고 표시만 한다. 다음 IsReady()에서 한 번만 다시 만든다 (OnEnter에서 타일 파일을 여러 개 불러와도 한 번)
		static void MarkDirty(Scene* scene) { mDirtyScene = scene; }
		static void Clear();

		// 지금 활성 씬으로 만든 맵인가 (다시 만들 일이 남아 있으면 여기서 만든다)
		static bool IsReady();

		// 맵 밖 읽기: 왼쪽·오른쪽은 가장자리 열이 계속 이어진 것으로, 위·아래는 빈 칸으로 본다
		static ePixel Get(int x, int y);
		static int GetWidth() { return mWidth; }
		static int GetHeight() { return mHeight; }

		// [left, right) 열 가운데 하나라도 row 줄이 Solid인가
		static bool IsSolidRow(int left, int right, int row);

		// [left, right) 열에서 fromRow~toRow 사이의 가장 위(가장 작은 줄) "윗면" 줄을 찾는다. 없으면 NoSurface.
		//  Solid 윗면  : 그 픽셀이 Solid이고 바로 위 픽셀은 Solid가 아님
		//  OneWay 윗면 : 그 픽셀이 OneWay이고 바로 위 픽셀이 비어 있음. oneWayFrom보다 위(작은 줄)의 발판은 무시
		//  둘 다 윗면 위로 16픽셀 안에 Solid가 없어야 한다 (벽 가장자리의 작은 틈은 윗면이 아님)
		// 같은 줄에 둘 다 있으면 Solid로 알려준다.
		static int FindSurface(int left, int right, int fromRow, int toRow, int oneWayFrom, ePixel* type);

		// 땅 속에서 빠져나올 곳: fromRow부터 위로 toRow까지 올라가며 처음 만나는(가장 가까운) Solid 윗면 줄
		static int FindSolidSurfaceAbove(int left, int right, int fromRow, int toRow);

		// 디버그 보기: 맵을 8비트 BMP로 저장 (빈 칸 검정, Solid 마젠타, OneWay 노랑)
		static bool SaveBmp(const std::wstring& path);

	private:
		static std::vector<unsigned char> mPixels;
		static int mWidth;
		static int mHeight;
		static Scene* mScene;
		static Scene* mDirtyScene;
	};
}
