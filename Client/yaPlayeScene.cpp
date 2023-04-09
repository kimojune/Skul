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
#include "yaPlayer.h"

namespace ya
{
	PlayeScene::PlayeScene()
			
	{

	}

	PlayeScene::~PlayeScene()
	{
	}

	void PlayeScene::Initialize()
	{
		Scene::Initialize();

		Chapter1 = Resources::Load<Sound>(L"Chapter1", L"..\\Resources\\Sound\\Chapter1.wav");
		//Chapter1->Play(true);
		
		object::Instantiate<PlayBG>(eLayerType::BG);
		mPlayer = object::Instantiate<Player>(Vector2(400.0f, 0.0f), eLayerType::Player);
		object::Instantiate<Monster>(Vector2(500.0f, 800.0f), eLayerType::Monster);
		object::Instantiate<Monster>(Vector2(400.0f, 800.0f), eLayerType::Monster);
		object::Instantiate<Ground>(Vector2(-100.0f, 800.0f), eLayerType::Ground);


		//Monster* monster = new Monster();
		//AddGameObeject(monster, eLayerType::Monster);


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
		const std::wstring& path = { L"..\\Tile\\test13" };
		TilePalatte::Load(path);

		Camera::SetTarget(mPlayer);
		Chapter1->Play(true);

		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Tile, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Monster, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Bullet, true);
		CollisionManager::SetLayer(eLayerType::Bullet, eLayerType::Monster, true);
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Ground, true);

	}
	void PlayeScene::OnExit()
	{

		Chapter1->Stop(true);
		Camera::SetTarget(nullptr);
		//mCuphead->SetPos(Vector2{ 0.0f, 0.0f });
	}
}
