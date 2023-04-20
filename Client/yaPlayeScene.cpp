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
		//Chapter1->Play(true);
		
		object::Instantiate<PlayBG>(Vector2(0.0f,0.0f),eLayerType::BG);

		object::Instantiate<BackCastle>(Vector2(500.0f,0.0f),eLayerType::Struct);
		object::Instantiate<Castle>(Vector2(0.0f,0.0f),eLayerType::Struct);


		//object::Instantiate<ItemGate>(Vector2(2200.0f, 1578.0f), eLayerType::Struct);
		object::Instantiate<BossGate>(Vector2(1600.0f, 1578.0f), eLayerType::Struct);



		mSkuls[(UINT)Skul::eSkulType::Basic] = object::Instantiate<Basic>(Vector2(0.0f, 450.0f), eLayerType::Player);
		mSkuls[(UINT)Skul::eSkulType::Nohead] = object::Instantiate<Nohead>(Vector2(0.0f, 0.0f), eLayerType::Player);
		
		object::Instantiate<SkulHead>(Vector2(0.0f, 0.0f), eLayerType::Bullet);
		
		object::Instantiate<SkulAttack>(Vector2(500.0f, 600.0f), eLayerType::Effect);

		object::Instantiate<Monster>(Vector2(500.0f, 500.0f), eLayerType::Monster);
		object::Instantiate<Monster>(Vector2(400.0f, 500.0f), eLayerType::Monster);


		object::Instantiate<Ground>(Vector2(0.0f, 1728.0f), eLayerType::Ground);

		for (Skul* skul : mSkuls)
		{
			if (skul == nullptr)
				continue;

			skul->SetState(ya::GameObject::eState::Pause);
		}


		mActiveSkul = mSkuls[(UINT)Skul::eSkulType::Basic];
		mActiveSkul->SetState(ya::GameObject::eState::Active);

		
	}

	void PlayeScene::Update()
	{
		if (Input::GetKeyState(eKeyCode::N) == eKeyState::Down)
		{
			SceneManager::LoadScene(eSceneType::Tool);
		}

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
		const std::wstring& path = { L"..\\Tile\\stage1" };
		TilePalatte::Load(path);

		Camera::SetTarget(mActiveSkul);
		//Chapter1->Play(true);

		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Tile, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Monster, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Bullet, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Struct, true);
		CollisionManager::SetLayer(eLayerType::Effect, eLayerType::Monster, true);
		CollisionManager::SetLayer(eLayerType::Bullet, eLayerType::Monster, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Ground, true);
		CollisionManager::SetLayer(eLayerType::Monster, eLayerType::Ground, true);
		CollisionManager::SetLayer(eLayerType::Monster, eLayerType::Tile, true);
		//CollisionManager::SetLayer(eLayerType::Bullet, eLayerType::Tile, true);

	}
	void PlayeScene::OnExit()
	{
		Chapter1->Stop(true);
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
