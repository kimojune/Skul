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
		//Skul* obj = dynamic_cast<Skul*>(other->GetOwner());

		//if (obj == nullptr)
		//{
		//	return;
		//}

		//Rigidbody* rb = obj->GetComponent<Rigidbody>();



		//Transform* objtr = obj->GetComponent<Transform>();
		//Vector2 objpos = objtr->GetPos();

		//Collider* objCol = obj->GetComponent<Collider>();
		//objpos.y -= (objCol->GetSize().y / 2);

		//Transform* pftr = GetComponent<Transform>();
		//Vector2 pfpos= pftr->GetPos();

		//if (pfpos.y > objpos.y)
		//{
		//	rb->SetGround(true);
		//	objpos.y = pfpos.y - (objCol->GetSize().y / 2); +1;
		//	objtr->SetPos(objpos);
		//}




	}
	void Platform::OnCollisionStay(Collider* other)
	{
		Skul* skul = dynamic_cast<Skul*>(other->GetOwner());
		if (skul == nullptr)
			return;

		Rigidbody* rb = skul->GetComponent<Rigidbody>();

		if (rb == nullptr)
		{
			return;
		}

		Vector2 velo = rb->GetVelocity();
		
		Transform* objtr = skul->GetComponent<Transform>();
		Vector2 objpos = objtr->GetPos();
		
		Collider* objCol = skul->GetComponent<Collider>();
		objpos.y -= (objCol->GetSize().y / 2);

		Transform* pftr = GetComponent<Transform>();
		Vector2 pfpos= pftr->GetPos();
		
		if (Input::GetKeyState(eKeyCode::DOWN) == eKeyState::Pressed)
		{
			objpos.y += 30.0f;
			objtr->SetPos(objpos);
			rb->SetGround(false);
			
		}

		if (velo.y > 0&& objpos.y + (objCol->GetSize().y / 2) < pfpos.y )
		{
			rb->SetGround(true);
			objpos.y = pfpos.y - (objCol->GetSize().y / 2) + 1;
			objtr->SetPos(objpos);
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