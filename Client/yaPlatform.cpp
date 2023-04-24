#include "yaPlatform.h"

#include "yaCollider.h"
#include "yaSkul.h"
#include "yaRigidbody.h"
#include "yaTransform.h"
#include "yaResources.h"
#include "yaImage.h"
#include "yaPlayeScene.h"
#include "yaSceneManager.h"
#include "yaCamera.h"
#include "yaTilePalatte.h"
#include "yaInput.h"

namespace ya
{
	Platform::Platform()
	{
	}

	Platform::~Platform()
	{
	}

	void Platform::Initialize()
	{

		mCollider = AddComponent<Collider>();
		mCollider->SetSize(Vector2(220.0f, 72.0f));


		GameObject::Initialize();
	}

	void Platform::Update()
	{
		GameObject::Update();


	}

	void Platform::Render(HDC hdc)
	{
		Transform* tr = GetComponent<Transform>();

		Vector2 renderPos = Camera::CaluatePos(tr->GetPos());


		GameObject::Render(hdc);


		//TransparentBlt(hdc, tr->GetPos().x, tr->GetPos().y
		//	, mImage->GetWidth(), mImage->GetHeight()
		//	, mImage->GetHdc()
		//	, 0, 0
		//	, mImage->GetWidth(), mImage->GetHeight()
		//	, RGB(255, 255, 255));

	}

	void Platform::Release()
	{
	}


	void Platform::OnCollisionEnter(Collider* other)
	{

		GameObject* obj = other->GetOwner();

		Rigidbody* rb = obj->GetComponent<Rigidbody>();

		if (rb == nullptr)
		{
			return;
		}

		rb->SetGround(true);


		//Collider* objCol = obj->GetComponent<Collider>();
		//Vector2 objPos = objCol->GetPos();

		//Collider* groundCol = this->GetComponent<Collider>();
		//Vector2 groundPos = groundCol->GetPos();

		//float fLen = fabs(objPos.y - groundPos.y);


		//float fSize = (objCol->GetSize().y / 2.0f); /*(groundCol->GetSize().y / 2.0f);*/

		//if (fLen < fSize)
		//{
		//	Transform* objTr = obj->GetComponent<Transform>();

		//	Vector2 objtrPos = objTr->GetPos();

		//	objtrPos.y -= (fSize - fLen) - 1.0f;
		//	objTr->SetPos(objtrPos);
		//}
	}
	void Platform::OnCollisionStay(Collider* other)
	{
		Skul* skul = dynamic_cast<Skul*>(other->GetOwner());

		Rigidbody* rb = skul->GetComponent<Rigidbody>();

		if (rb == nullptr)
		{
			return;
		}

		

		Collider* skulCol = skul->GetComponent<Collider>();
		Vector2 skulPos = skulCol->GetPos();

		Collider* groundCol = this->GetComponent<Collider>();
		Vector2 groundPos = groundCol->GetPos();

		//if (skulPos.y < groundPos.y)
		//{
		//	rb->SetGround(true);
		//}

		if (Input::GetKeyState(eKeyCode::DOWN) == eKeyState::Down)
		{
			rb->SetGround(false);
		}


	}
	void Platform::OnCollisionExit(Collider* other)
	{
		Skul* skul = dynamic_cast<Skul*>(other->GetOwner());


		Rigidbody* rb = other->GetOwner()->GetComponent<Rigidbody>();
		//Rigidbody* rb = skul->GetComponent<Rigidbody>();
		if (rb == nullptr)
			return;
		rb->SetGround(false);

	}
}