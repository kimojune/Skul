#include "yaTile.h"
#include "yaTransform.h"
#include "yaCamera.h"
#include "yaCollider.h"
#include "yaSkul.h"
#include "yaRigidbody.h"
#include "yaTransform.h"
namespace ya
{
	Tile::Tile()
		:mAtlas(nullptr)
		,mIndex(-1)
		,mX(-1)
		,mY(-1)
	{
	}
	Tile::Tile(Vector2 pos)
		:mAtlas(nullptr)
		, mIndex(-1)
		, mX(-1)
		, mY(-1)
	{
		GetComponent<Transform>()->SetPos(pos);

	}
	Tile::~Tile()
	{
	}
	void Tile::InitializeTile(Image* atlas, int index)
	{
		mIndex = index;
		if (atlas == nullptr || index < 0)
			return;
		

		mAtlas = atlas;
		SetIndex(index);

		GameObject::Initialize();
	}

	void Tile::SetIndex(int index)
	{
		int maxCol = mAtlas->GetWidth() / TILE_SIZE_X;
		int maxRow = mAtlas->GetHeight() / TILE_SIZE_Y;

		mY = index / maxCol;
		mX = index % maxRow;
	}
	void Tile::Update()
	{
		GameObject::Update();

		
	}
	void Tile::Render(HDC hdc)
	{
		if (mAtlas == nullptr || mIndex < 0)
			return;

		Transform* tr = GetComponent<Transform>();

		Vector2 renderPos = Camera::CaluatePos(tr->GetPos());

		TransparentBlt(hdc
			, renderPos.x, renderPos.y
			, TILE_SIZE_X, TILE_SIZE_Y
			, mAtlas->GetHdc()
			, TILE_SIZE_X * mX, TILE_SIZE_Y * mY
			, TILE_SIZE_X, TILE_SIZE_X
			, RGB(255, 0, 255));

		GameObject::Render(hdc);
	}

	void Tile::OnCollisionEnter(Collider* other)
	{
		// 착지는 타일 그림으로 만든 픽셀 충돌 맵으로 한다 (yaCollisionMap, Rigidbody).
		// 타일에는 콜라이더가 없고, 여기서 위치를 옮기면 맵 판정과 어긋나므로 아무것도 하지 않는다.
	}

	void Tile::OnCollisionStay(Collider* other)
	{
	}

	void Tile::OnCollisionExit(Collider* other)
	{
		//GameObject* obj = other->GetOwner();

		//Rigidbody* rb = obj->GetComponent<Rigidbody>();
		//if (rb == nullptr)
		//	return;

		//rb->SetGround(false);
	}


}