#include "yaCollider.h"
#include "yaTransform.h"
#include "yaGameObject.h"
#include "yaCamera.h"

namespace ya
{
	UINT Collider::ColliderNumber = 0;
	Collider::Collider()
		: Component(eComponentType::Collider)
		, mCenter(Vector2::Zero)
		, mPos(Vector2::Zero)
		, mSize(100.0f, 100.0f)
		, mID(ColliderNumber++)
		, mCollisionCount(0)

	{

	}

	Collider::~Collider()
	{
	}
	void Collider::Initialize()
	{
		Transform* tr = GetOwner()->GetComponent<Transform>();
		mPos = tr->GetPos() + mCenter;
	}
	void Collider::Update()
	{
	Transform* tr = GetOwner()->GetComponent<Transform>();
		mPos = tr->GetPos() + mCenter;
	}
	
	void Collider::Render(HDC hdc)
	{
		// 충돌 상자 디버그 그리기 (꺼 둠). 부딪히는 중이면 빨강, 아니면 초록 2px 테두리
		//COLORREF color = (mCollisionCount <= 0) ? RGB(0, 255, 0) : RGB(255, 0, 0);
		//Vector2 pos = Camera::CaluatePos(mPos);
		//Renderer::DrawRect((int)pos.x, (int)pos.y, (int)mSize.x, (int)mSize.y, color, 2);

		//mCollisionCount = 0;
	}
	void Collider::Release()
	{
	}
	void Collider::OnCollisionEnter(Collider* other)
	{
		GetOwner()->OnCollisionEnter(other);
	}
	void Collider::OnCollisionStay(Collider* other)
	{
		mCollisionCount= 1;

		GetOwner()->OnCollisionStay(other);
	}
	void Collider::OnCollisionExit(Collider* other)
	{

		GetOwner()->OnCollisionExit(other);
	}
}
