#include "yaBossBullet.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaCollider.h"
#include "yaTime.h"
#include "yaSound.h"
#include "yaResources.h"

namespace ya
{
	BossBullet::BossBullet()
	{
	}
	BossBullet::~BossBullet()
	{
	}
	void BossBullet::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 BulletPos = tr->GetPos();
		BulletPos.x += 150.0f;
		BulletPos.y += 200.0f;

		tr->SetScale(Vector2(1.8f, 1.8f));
		tr->SetPos(BulletPos);
		mPrevPos = BulletPos;
		
		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimations(L"..\\Resources\\Boss\\Effect\\Bullet\\Bullet\\Impact", Vector2(-14.0f, -8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Boss\\Effect\\Bullet\\Bullet\\Loop", Vector2(-14.0f, -8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Boss\\Effect\\Bullet\\Bullet\\Outro", Vector2(-14.0f, -8.0f), 0.1f);

		ElderEnt_EnergyBomb_Ready = Resources::Load<Sound>(L"ElderEnt_EnergyBomb_Ready", L"..\\Resources\\Sound\\Boss\\ElderEnt_EnergyBomb_Ready.wav");
		ElderEnt_EnergyBomb_Fire = Resources::Load<Sound>(L"ElderEnt_EnergyBomb_Fire", L"..\\Resources\\Sound\\Boss\\ElderEnt_EnergyBomb_Fire.wav");

		mAnimator->Play(L"BulletLoop", true);
		mPlayed = false;
		mSound= false;
		
		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(125.0f, 125.0f));
		collider->SetCenter(Vector2(10.0f, -75.0f));

		GameObject::Initialize();

	}
	void BossBullet::Update()
	{
		if (mPlayed == false)
		{
			ElderEnt_EnergyBomb_Ready->Play(false);
			mAnimator->Play(L"BulletLoop", true);
			mSound = false;
			mPlayed = true;
			switch (mDirect)
			{
			case eDirection::Left:
				direction = Vector2(-1.0f, 0.0f);
				break;
			case eDirection::Right:
				direction = Vector2(1.0f, 0.0f);
				break;
			case eDirection::Up:
				direction = Vector2(0.0f, -1.0f);
				break;
			case eDirection::Down:
				direction = Vector2(0.0f, 1.0f);
				break;
			case eDirection::LeftUp:
				direction = Vector2(-1.0f, -1.0f);
				break;
			case eDirection::RightUp:
				direction = Vector2(1.0f, -1.0f);
				break;
			case eDirection::LeftDown:
				direction = Vector2(-1.0f, 1.0f);
				break;
			case eDirection::RightDown:
				direction = Vector2(1.0f, 1.0f);
				break;
			case eDirection::End:
				break;
			default:
				break;
			}
		}
		
		mTime += Time::DeltaTime();

		if (mTime > 3)
		{
			if (!(mSound))
			{
				ElderEnt_EnergyBomb_Fire->Play(true);
				mSound = true;
			}
			Transform* tr = GetComponent<Transform>();
			Vector2 bulletpos = tr->GetPos();
			bulletpos += direction * 500 * Time::DeltaTime();
			tr->SetPos(bulletpos);
		}

		if (mTime > 6)
		{
			SetState(eState::Pause);
			mTime = 0;
			ElderEnt_EnergyBomb_Fire->Stop(true);
			mSound = true;
		}

		GameObject::Update();

	}
	void BossBullet::Render(HDC hdc)
	{

		GameObject::Render(hdc);

	}
	void BossBullet::Release()
	{
		GameObject::Release();

	}
	void BossBullet::SetPrevPos()
	{
		Transform* tr = GetComponent<Transform>();
		tr->SetPos(mPrevPos);
	}
	void BossBullet::OnCollisionEnter(Collider* other)
	{
	}
	void BossBullet::OnCollisionStay(Collider* other)
	{
	}
	void BossBullet::OnCollisionExit(Collider* other)
	{
	}
	void BossBullet::Impact()
	{
	}
	void BossBullet::Loop()
	{
	}
	void BossBullet::Outro()
	{
	}
}