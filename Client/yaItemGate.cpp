#include "yaItemGate.h"
#include "yaAnimator.h"
#include "yaSkul.h"
#include "yaCollider.h"
#include "yaInput.h"
#include "yaSceneManager.h"
#include "yaTransform.h"
namespace ya
{
	ItemGate::ItemGate()
	{
	}
	ItemGate::~ItemGate()
	{
	}
	void ItemGate::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.0f, 2.0f));

		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(250, 300));
		collider->SetCenter(Vector2(0, -75));

		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimations(L"..\\Resources\\Object\\Gate\\ItemGate\\DeActivate", Vector2(-14.0f, -8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Object\\Gate\\ItemGate\\Activate", Vector2(-14.0f, -8.0f), 0.1f);

		mAnimator->Play(L"ItemGateDeActivate", false);

		GameObject::Initialize();
	}
	void ItemGate::Update()
	{
		GameObject::Update();
	}
	void ItemGate::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void ItemGate::Release()
	{
		GameObject::Release();
	}
	void ItemGate::OnCollisionEnter(Collider* other)
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 gatepos = tr->GetPos();

		gatepos.x -= 20.0f;
		tr->SetPos(gatepos);

		mbActivate = true;
		Skul* skul = dynamic_cast<Skul*>(other->GetOwner());
		if (skul == nullptr)
			return;
		if (mbActivate == true)
			mAnimator->Play(L"ItemGateActivate", true);
	}
	void ItemGate::OnCollisionStay(Collider* other)
	{
		Skul* skul = dynamic_cast<Skul*>(other->GetOwner());
		if (skul == nullptr)
			return;

		if (mbActivate)
		{
			if (Input::GetKeyState(eKeyCode::F) == eKeyState::Down)
			{
				SceneManager::LoadScene(eSceneType::Stage2);
			}
		}
	}
	void ItemGate::OnCollisionExit(Collider* other)
	{
		mbActivate == false;

		mAnimator->Play(L"ItemGateDeActivate", false);

		Transform* tr = GetComponent<Transform>();
		Vector2 gatepos = tr->GetPos();

		gatepos.x += 20.0f;
		tr->SetPos(gatepos);
	}
}