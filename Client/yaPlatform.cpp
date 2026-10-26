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
		// 발판 사각형. 씬에 들어갈 때 CollisionMap이 이 사각형에 걸친 타일 그림을 OneWay 발판으로 칠한다.
		// 착지는 Rigidbody가 맵으로 하므로 충돌 콜백에서는 아무것도 하지 않는다.
		mCollider = AddComponent<Collider>();
		mCollider->SetSize(Vector2(220.0f, 72.0f));


		GameObject::Initialize();
	}

	void Platform::Update()
	{
		GameObject::Update();

		// ↓: 플레이어가 발판(OneWay) 위에 서 있으면 아래로 내려간다. 단단한 땅 위에서는 아무 일도 없다.
		// 발판이 여러 개여도 DropThrough는 한 번만 효과가 있다 (착지 상태일 때만 동작).
		if (!Input::GetKeyDown(eKeyCode::DOWN) && !Input::GetKey(eKeyCode::DOWN))
			return;

		PlayeScene* playScene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
		Skul* skul = (playScene != nullptr) ? playScene->GetSkul() : nullptr;
		if (skul == nullptr)
			return;

		Rigidbody* rb = skul->GetComponent<Rigidbody>();
		if (rb != nullptr)
			rb->DropThrough();
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
		// 예전에는 Stay에서만 착지시켜서 빠르게 떨어지면 뚫고 지나갈 수 있었다.
		// 이제 착지와 ↓ 내려가기는 픽셀 충돌 맵 기준이다 (Rigidbody, Platform::Update).
	}
	void Platform::OnCollisionExit(Collider* other)
	{
	}
}