#include "yaSkulAttack.h"
#include "yaCollider.h"
#include "yaTransform.h"
#include "yaPlayeScene.h"
#include "yaSceneManager.h"
#include "yaSkul.h"
namespace ya
{
	SkulAttack::SkulAttack()
	{
		SetName(L"SkulAttack");
	}
	SkulAttack::~SkulAttack()
	{
	}
	void SkulAttack::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		PlayeScene* playscene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
		
		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(150.0f, 145.0f));
		collider->SetCenter(Vector2(-65.0f, -110.0f));


		GameObject::Initialize();
	}
	void SkulAttack::Update()
	{
		Transform* tr = GetComponent<Transform>();
		PlayeScene* playscene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
		Skul* mActiveSkul = playscene->GetSkul();
		
		Vector2 skulPos = mActiveSkul->GetComponent<Transform>()->GetPos();

		mDirect = mActiveSkul->GetDirect();

		if (mDirect == eDirection::Left)
			skulPos.x -= 20.0f;
		else
			skulPos.x += 10.0f;
		
		tr->SetPos(skulPos);

		GameObject::Update();
	}
	void SkulAttack::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void SkulAttack::Release()
	{
		GameObject::Release();
	}
	void SkulAttack::OnCollisionEnter(Collider* other)
	{
		
	}
	void SkulAttack::OnCollisionStay(Collider* other)
	{
	}
	void SkulAttack::OnCollisionExit(Collider* other)
	{
	}
}