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

 		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(36.0f, 36.0f));

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
		Skul* skul = dynamic_cast<Skul*>(other->GetOwner());

		if (skul == nullptr)
			return;

		Rigidbody* rb = skul->GetComponent<Rigidbody>();
		rb->SetGround(true);


		Collider* skulCol = skul->GetComponent<Collider>();
		Vector2 skulPos = skulCol->GetPos();

		Collider* groundCol = this->GetComponent<Collider>();
		Vector2 groundPos = groundCol->GetPos();

		float fLen = fabs(skulPos.y - groundPos.y);

		int a = 0;

		float fSize = (skulCol->GetSize().y / 2.0f) + (groundCol->GetSize().y / 2.0f);

		if (fLen < fSize)
		{
			Transform* skulTr = skul->GetComponent<Transform>();
			Transform* grTr = this->GetComponent<Transform>();

			Vector2 skulPos = skulTr->GetPos();
			Vector2 grPos = grTr->GetPos();

			skulPos.y -= (fSize - fLen) - 1.0f;
			skulTr->SetPos(skulPos);
		}
	}

	void Tile::OnCollisionStay(Collider* other)
	{
	}

	void Tile::OnCollisionExit(Collider* other)
	{
		Skul* skul = dynamic_cast<Skul*>(other->GetOwner());

		if (skul == nullptr)
			return;

		Rigidbody* rb = skul->GetComponent<Rigidbody>();
		rb->SetGround(false);
	}


}