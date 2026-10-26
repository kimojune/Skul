#include "yaEntSlamHitbox.h"
#include "yaCollider.h"
#include "yaTransform.h"
#include "yaTime.h"
#include "yaScene.h"
#include "yaSceneManager.h"
#include "yaSkul.h"
#include "yaPlayerStats.h"

namespace ya
{
	EntSlamHitbox* EntSlamHitbox::Spawn(Vector2 bottomCenter, Vector2 size, int damage, Vector2 sourcePos, float lifetime)
	{
		// 적의 공격이라 EnemyBullet 레이어 (Player 와 충돌하도록 PlayeScene::OnEnter 에서 켜져 있다)
		EntSlamHitbox* hitbox = new EntSlamHitbox();
		hitbox->mSize = size;
		hitbox->mDamage = damage;
		hitbox->mSourcePos = sourcePos;
		hitbox->mLifetime = lifetime;

		Scene* scene = SceneManager::GetActiveScene();
		scene->AddGameObeject(hitbox, eLayerType::EnemyBullet);
		hitbox->GetComponent<Transform>()->SetPos(bottomCenter);
		hitbox->Initialize();
		return hitbox;
	}

	EntSlamHitbox::EntSlamHitbox()
		: mSize(Vector2::Zero)
		, mSourcePos(Vector2::Zero)
		, mDamage(0)
		, mLifetime(0.0f)
		, mTime(0.0f)
		, mbHit(false)
	{
		SetName(L"EntSlamHitbox");
	}
	EntSlamHitbox::~EntSlamHitbox()
	{
	}
	void EntSlamHitbox::Initialize()
	{
		// Collider 의 위치는 왼쪽 위 기준이라, Transform(바닥 가운데)에서 (-w/2, -h) 만큼 옮긴다
		Collider* collider = AddComponent<Collider>();
		collider->SetSize(mSize);
		collider->SetCenter(Vector2(-mSize.x / 2.0f, -mSize.y));

		GameObject::Initialize();
	}
	void EntSlamHitbox::Update()
	{
		mTime += (float)Time::DeltaTime();
		if (mTime >= mLifetime)
			SetState(eState::Death);

		GameObject::Update();
	}
	void EntSlamHitbox::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void EntSlamHitbox::Release()
	{
		GameObject::Release();
	}
	void EntSlamHitbox::OnCollisionEnter(Collider* other)
	{
		tryHit(other);
	}
	void EntSlamHitbox::OnCollisionStay(Collider* other)
	{
		tryHit(other);
	}
	void EntSlamHitbox::OnCollisionExit(Collider* other)
	{
	}
	void EntSlamHitbox::tryHit(Collider* other)
	{
		if (mbHit || GetState() != eState::Active)
			return;

		if (dynamic_cast<Skul*>(other->GetOwner()) == nullptr)
			return;

		if (PlayerStats::TakeDamage(mDamage, mSourcePos))
			mbHit = true;
	}
}
