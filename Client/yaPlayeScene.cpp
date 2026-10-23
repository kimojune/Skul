#pragma once

#include "yaPlayeScene.h"
#include "yaSkul.h"
#include "yaInput.h"
#include "yaSceneManager.h"
#include "yaMonster.h"
#include "yaCollisionManager.h"
#include "yaPlayBG.h"
#include "yaTransform.h"
#include "yaCamera.h"
#include "yaObject.h"
#include "yaGround.h"
#include "yaSound.h"
#include "yaResources.h"
#include "yaTilePalatte.h"
#include "yaBasicSkul.h"
#include "yaNoheadSkul.h"
#include "yaGameObject.h"
#include "yaSkulAttack.h"
#include "yaStruct.h"
#include "yaCastle.h"
#include "yaBackCastle.h"
#include "yaBossGate.h"
#include "yaItemGate.h"
#include "yaRigidbody.h"
#include "yaTime.h"
#include "yaPlayerStats.h"
#include "yaPlayerHUD.h"
#include "yaSkulHead.h"

namespace ya
{
	static Skul* mSkuls[(UINT)Skul::eSkulType::End] = {};


	PlayeScene::PlayeScene()
		: Chapter1(nullptr)
		, Chapter1_BossBG(nullptr)
		, mActiveSkul(nullptr)
		, mSkulHead(nullptr)
	{

	}

	PlayeScene::~PlayeScene()
	{

	}

	void PlayeScene::Initialize()
	{
		Scene::Initialize();

		//mPixelMap = TilePalatte::GetTilePixel();
		
		Chapter1 = Resources::Load<Sound>(L"Chapter1", L"..\\Resources\\Sound\\Chapter1.wav");
		Chapter1_BossBG = Resources::Load<Sound>(L"Chapter1_Boss", L"..\\Resources\\Sound\\Chapter1_Boss.wav");
		//Chapter1->Play(true);
		
		mSkulHead = object::Instantiate<SkulHead>(Vector2(0.0f, 0.0f), eLayerType::Bullet);
		object::Instantiate<SkulAttack>(Vector2(500.0f, 600.0f), eLayerType::Effect);

		// 좌하단 HP 바·스킬 HUD (UI 레이어는 가장 나중에 그려진다)
		object::Instantiate<PlayerHUD>(eLayerType::UI);

		// 적·보스가 PlayerStats::TakeDamage로 준 피해를 지금 조작 중인 스컬에게 전달한다 (피격 반응, 사망).
		// 콜백은 하나뿐이라 플레이 씬마다 같은 함수를 다시 등록해도 된다
		PlayerStats::SetOnDamaged([](int damage, Vector2 sourcePos)
			{
				PlayeScene* playScene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
				if (playScene == nullptr || playScene->GetSkul() == nullptr)
					return;

				playScene->GetSkul()->OnDamaged(damage, sourcePos);
			});
	}

	void PlayeScene::Update()
	{
		//if (Input::GetKeyState(eKeyCode::N) == eKeyState::Down)
		//{
		//	SceneManager::LoadScene(eSceneType::Stage2);
		//}

		Scene::Update();

		PlayerStats::Update((float)Time::DeltaTime());

		// 사망 연출이 끝나면 HP를 채우고 타이틀로 돌아간다.
		// 씬 전환은 모든 오브젝트 Update가 끝난 여기서 한다 (Update 도중에 바꾸면 뒤에 도는 오브젝트가 타이틀 씬을 플레이 씬으로 착각한다).
		// Phase 3에서 결과 화면과 스테이지 리셋으로 바꾼다. 지금은 스테이지 상태(몬스터 등)가 그대로 남는다.
		if (mActiveSkul != nullptr && mActiveSkul->IsDeathFinished())
		{
			mActiveSkul->ResetForNewRun();
			PlayerStats::Reset();
			Camera::SetTarget(nullptr);	// BossStage::OnExit는 PlayeScene::OnExit를 부르지 않는다
			SceneManager::LoadScene(eSceneType::Title);
			Camera::FadeIn(Skul::DeadFadeTime);
		}
	}

	void PlayeScene::Render(HDC hdc)
	{
		//Super::Tick();
		Scene::Render(hdc);
	}

	void PlayeScene::Release()
	{
		Scene::Release();
	}
	void PlayeScene::OnEnter()
	{

		Camera::SetTarget(mActiveSkul);
		Rigidbody* rd = mActiveSkul->GetComponent<Rigidbody>();
		
		rd->SetGround(false);

		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Tile, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Monster, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Bullet, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::EnemyBullet, true);
		CollisionManager::SetLayer(eLayerType::Ground, eLayerType::EnemyBullet, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Struct, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Gate, true);
		CollisionManager::SetLayer(eLayerType::Effect, eLayerType::Monster, true);
		CollisionManager::SetLayer(eLayerType::Effect, eLayerType::Boss, true);
		CollisionManager::SetLayer(eLayerType::Bullet, eLayerType::Monster, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Ground, true);
		CollisionManager::SetLayer(eLayerType::Monster, eLayerType::Ground, true);
		CollisionManager::SetLayer(eLayerType::Monster, eLayerType::Tile, true);
		
		//CollisionManager::SetLayer(eLayerType::Bullet, eLayerType::Tile, true);

	}
	void PlayeScene::OnExit()
	{
		// 머리는 씬마다 따로 있어서 던진 채로 씬을 옮기면 다음 씬에서 찾을 수 없다. 나가기 전에 몸으로 돌려 둔다
		if (PlayerStats::IsHeadOut())
		{
			Nohead* nohead = dynamic_cast<Nohead*>(mSkuls[(UINT)Skul::eSkulType::Nohead]);
			if (nohead != nullptr && nohead->GetState() == GameObject::eState::Active)
				nohead->TakeHeadBack(false, false);
			else if (mSkulHead != nullptr)
				mSkulHead->Hide();
			PlayerStats::OnHeadRetrieved(false);
		}

		Camera::SetTarget(nullptr);
		//mCuphead->SetPos(Vector2{ 0.0f, 0.0f });
	}

	void PlayeScene::SetSkul(Skul::eSkulType type)
	{
		mActiveSkul = mSkuls[(UINT)type];
		mActiveSkul->SetState(ya::GameObject::eState::Active);
		Camera::SetTarget(mActiveSkul);
	}
}
