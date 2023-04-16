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


		GameObject* obj = other->GetOwner();

		Rigidbody* rb = obj->GetComponent<Rigidbody>();

		if (rb == nullptr)
		{
			int a = 0;
			return;
			
		}
	
		rb->SetGround(true);


		Collider* objCol = obj->GetComponent<Collider>();
		Vector2 objPos = objCol->GetPos();

		Collider* groundCol = this->GetComponent<Collider>();
		Vector2 groundPos = groundCol->GetPos();

		float fLen = fabs(objPos.y - groundPos.y);

		int a = 0;

		float fSize = (objCol->GetSize().y / 2.0f) + (groundCol->GetSize().y / 2.0f);

		if (fLen < fSize)
		{
			Transform* objTr = obj->GetComponent<Transform>();
			Transform* grTr = this->GetComponent<Transform>();

			Vector2 objPos = objTr->GetPos();
			Vector2 grPos = grTr->GetPos();

			objPos.y -= (fSize - fLen) - 1.0f;
			objTr->SetPos(objPos);
		}
	}
	void Ground::OnCollisionStay(Collider* other)
	{

	}
	void Ground::OnCollisionExit(Collider* other)
	{
		//Skul* skul = dynamic_cast<Skul*>(other->GetOwner());


		Rigidbody* rb = other->GetOwner()->GetComponent<Rigidbody>();
		//Rigidbody* rb = skul->GetComponent<Rigidbody>();
		if (rb == nullptr)
			return;
		rb->SetGround(false);

	}
}