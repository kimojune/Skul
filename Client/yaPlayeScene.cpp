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

namespace ya
{
	static Skul* mSkuls[(UINT)Skul::eSkulType::End] = {};


	PlayeScene::PlayeScene()
			
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
		
		object::Instantiate<SkulHead>(Vector2(0.0f, 0.0f), eLayerType::Bullet);
		object::Instantiate<SkulAttack>(Vector2(500.0f, 600.0f), eLayerType::Effect);

	}

	void PlayeScene::Update()
	{
		//if (Input::GetKeyState(eKeyCode::N) == eKeyState::Down)
		//{
		//	SceneManager::LoadScene(eSceneType::Stage2);
		//}

		Scene::Update();
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
