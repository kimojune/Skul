#include "yaGameObject.h"
#include "yaTransform.h"

namespace ya
{
	GameObject::GameObject()
		:mState(eState::Active)
	{
		mComponents.resize((UINT)eComponentType::End);
		AddComponent<Transform>();
	}

	GameObject::~GameObject()
	{
		for (Component* comp : mComponents)
		{
			delete comp;
			comp = nullptr;
		}
	}

	void GameObject::Initialize()
	{
		for (Component* comp : mComponents)
		{
			if (comp == nullptr)
				continue;

			comp->Initialize();
		}
	}

	void GameObject::Update()
	{
		// Collider는 Rigidbody·Animator 등이 이번 프레임에 위치를 옮긴 뒤에 따라가도록
		// 맨 마지막에 갱신한다. (먼저 갱신하면 충돌 검사가 한 프레임 전 위치로 돼서
		// 착지할 때 한 프레임 동안 지면에 파고든다)
		for (Component* comp : mComponents)
		{
			if (comp == nullptr)
				continue;

			if (comp->GetType() == eComponentType::Collider)
				continue;

			comp->Update();
		}

		Component* collider = mComponents[(UINT)eComponentType::Collider];
		if (collider != nullptr)
			collider->Update();
	}

	void GameObject::Render(HDC hdc)
	{
		for (Component* comp : mComponents)
		{
			if (comp == nullptr)
				continue;

			comp->Render(hdc);
		}
	}

	void GameObject::Release()
	{

	}
	void GameObject::OnCollisionEnter(Collider* other)
	{
	}
	void GameObject::OnCollisionStay(Collider* other)
	{
	}
	void GameObject::OnCollisionExit(Collider* other)
	{
	}
}
