#include "yaBossGate.h"
#include "yaAnimator.h"
#include "yaCollider.h"
#include "yaTransform.h"
#include "yaInput.h"
#include "yaSceneManager.h"
#include "yaSkul.h"
#include "yaResources.h"
#include "yaCamera.h"

namespace ya
{
	BossGate::BossGate()
		:mbActivate(false)
	{
	}
	BossGate::~BossGate()
	{
	}
	void BossGate::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.0f, 2.0f));

		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(250, 300));
		collider->SetCenter(Vector2(0, -75));

		mAnimator = AddComponent<Animator>();
		mImage = Resources::Load<Image>(L"BossGate", L"..\\Resources\\Object\\Gate\\BossGate\\DeActivate\\BossGate.bmp");

		mAnimator->CreateAnimations(L"..\\Resources\\Object\\Gate\\BossGate\\DeActivate", Vector2(-14.0f, -8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Object\\Gate\\BossGate\\Activate", Vector2(-14.0f, -8.0f), 0.1f);
		
		mAnimator->Play(L"BossGateDeActivate", false);
		GameObject::Initialize();

	}
	void BossGate::Update()
	{

		GameObject::Update();

	}
	void BossGate::Render(HDC hdc)
	{



		GameObject::Render(hdc);

	}
	void BossGate::Release()
	{
		GameObject::Release();

	}
	void BossGate::OnCollisionEnter(Collider* other)
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 gatepos = tr->GetPos();

		gatepos.x -= 20.0f;
		tr->SetPos(gatepos);

		mbActivate = true;
		Skul* skul = dynamic_cast<Skul*>(other->GetOwner());
		if(skul==nullptr)
			return;
		if (mbActivate == true)
			mAnimator->Play(L"BossGateActivate", true);
	}
	void BossGate::OnCollisionStay(Collider* other)
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
	void BossGate::OnCollisionExit(Collider* other)
	{
		mbActivate == false;

		mAnimator->Play(L"BossGateDeActivate", false);

		Transform* tr = GetComponent<Transform>();
		Vector2 gatepos = tr->GetPos();

		gatepos.x += 20.0f;
		tr->SetPos(gatepos);
	}
}
