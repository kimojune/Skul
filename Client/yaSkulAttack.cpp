#include "yaSkulAttack.h"
#include "yaCollider.h"
#include "yaTransform.h"
#include "yaPlayeScene.h"
#include "yaSceneManager.h"
#include "yaSkul.h"
#include "yaMonster.h"
#include "yaAnimator.h"

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
		
		if (playscene == nullptr)
			return;

		mSkul = playscene->GetSkul();
		
		Vector2 skulPos = mSkul->GetComponent<Transform>()->GetPos();

		mDirect = mSkul->GetDirect();

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
		//Monster* monster = dynamic_cast<Monster*>(other->GetOwner());
	}
	void SkulAttack::OnCollisionStay(Collider* other)
	{
		Monster* monster = dynamic_cast<Monster*>(other->GetOwner());
		
		if (monster == nullptr)
			return;

		Skul::eSkulState skulstate = mSkul->GetSkulState();
		if (Skul::eSkulState::Attack == skulstate
			|| Skul::eSkulState::JumpAttack == skulstate)
		{
			if (monster->GetMonsterState() == Monster::eMonsterState::Hit)
				return;
			monster->SetMonsterState(Monster::eMonsterState::Hit);
			Animator* monAnimator = monster->GetComponent<Animator>();
		}

	}
	void SkulAttack::OnCollisionExit(Collider* other)
	{

	}
}