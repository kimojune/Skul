#include "yaGround.h"
#include "yaCollider.h"
#include "yaSkul.h"
#include "yaRigidbody.h"
#include "yaTransform.h"
#include "yaResources.h"
#include "yaImage.h"
#include "yaPlayeScene.h"
#include "yaSceneManager.h"
#include "yaCamera.h"

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
		mCollider->SetSize(Vector2(3200.0f, 500.0f));   
		//mCollider->SetCenter(Vector2(0.0f, 250.0f));   
		//
		//Scene* scene = SceneManager::GetActiveScene();
		//PlayeScene* playscene = dynamic_cast<PlayeScene*>(scene);

		//mImage = playscene->GetPixMap();
		
		GameObject::Initialize();
	}

	void Ground::Update()
	{
		GameObject::Update();

	/*	Transform* playerTr = mPlayer->GetComponent<Transform>();

		COLORREF color = mImage->GetPixel(playerTr->GetPos().x, playerTr->GetPos().y);

		Rigidbody* rb = mPlayer->GetComponent<Rigidbody>();
		if (color == RGB(255, 0, 255))
		{
			rb->SetGround(true);

			Vector2 pos = playerTr->GetPos();
			pos.y -= 1;
			playerTr->SetPos(pos);
		}
		else
		{
			rb->SetGround(false);
		}*/
	}

	void Ground::Render(HDC hdc)
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

	void Ground::Release()
	{
	}


	void Ground::OnCollisionEnter(Collider* other)
	{

		GameObject* obj = other->GetOwner();

		Rigidbody* rb = obj->GetComponent<Rigidbody>();

		if (rb == nullptr)
		{
			return;
		}
	
		rb->SetGround(true);


		Collider* objCol = obj->GetComponent<Collider>();
		Vector2 objPos = objCol->GetPos();

		Collider* groundCol = this->GetComponent<Collider>();
		Vector2 groundPos = groundCol->GetPos();

		float fLen = fabs(objPos.y - groundPos.y);


		float fSize = (objCol->GetSize().y / 2.0f); /*(groundCol->GetSize().y / 2.0f);*/

		if (fLen < fSize)
		{
			Transform* objTr = obj->GetComponent<Transform>();

			Vector2 objtrPos = objTr->GetPos();

			objtrPos.y -= (fSize - fLen) - 1.0f;
			objTr->SetPos(objtrPos);
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