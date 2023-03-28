#include "yaGround.h"
#include "yaCollider.h"
#include "yaSkul.h"
#include "yaRigidbody.h"
#include "yaTransform.h"

namespace ya
{
	Ground::Ground()
	{
	}

	Ground::~Ground()
	{
	}

	void Ground::Initialize()
	{
		mCollider = AddComponent<Collider>();
		mCollider->SetSize(Vector2(3200.0f, 50.0f));            

		GameObject::Initialize();
	}

	void Ground::Update()
	{
		GameObject::Update();
	}

	void Ground::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}

	void Ground::Release()
	{
	}


	void Ground::OnCollisionEnter(Collider* other)
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
	void Ground::OnCollisionStay(Collider* other)
	{

	}
	void Ground::OnCollisionExit(Collider* other)
	{
		Skul* skul = dynamic_cast<Skul*>(other->GetOwner());

		if (skul == nullptr)
			return;

		Rigidbody* rb = skul->GetComponent<Rigidbody>();
		rb->SetGround(false);

	}
}