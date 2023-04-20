#include "yaGate.h"
#include "yaAnimator.h"
#include "yaCollider.h"
#include "yaTransform.h"
#include "yaInput.h"
#include "yaSceneManager.h"
#include "yaSkul.h"

namespace ya
{
	Gate::Gate()
		:mOpen(false)
	{
	}
	Gate::~Gate()
	{
	}
	void Gate::Close()
	{
	}
	void Gate::Open()
	{
		mAnimator->Play(L"Activate", true);
		if (Input::GetKeyState(eKeyCode::F) == eKeyState::Down)
		{
			SceneManager::LoadScene(eSceneType::Play2);
		}
	}
	void Gate::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.0f, 2.0f));
		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(250, 300));
		collider->SetCenter(Vector2(-10, -75));
		
		mAnimator = GetComponent<Animator>();
		
	
		mAnimator->Play(L"DeActivate", false);
		GameObject::Initialize();

	}
	void Gate::Update()
	{
		if (mOpen)
			Open();
		else
			Close();
		GameObject::Update();
	}
	void Gate::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void Gate::Release()
	{
		GameObject::Release();
	}
	void Gate::OnCollisionEnter(Collider* other)
	{
		Skul* skul = dynamic_cast<Skul*>(other->GetOwner());
		if (skul == nullptr)
			return;

		mOpen = true;
	}
	void Gate::OnCollisionStay(Collider* other)
	{
	}
	void Gate::OnCollisionExit(Collider* other)
	{
		mOpen = false;
	}

}
