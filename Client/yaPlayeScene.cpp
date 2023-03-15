#include "yaPlayeScene.h"
#include "yaSkul.h"
#include "yaInput.h"
#include "yaSceneManager.h"
#include "yaMonster.h"
#include "yaCollisionManager.h"
#include "yaBG.h"
#include "yaTransform.h"
#include "yaCamera.h"

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

		BG* mBG = new BG(eSceneType::Play);
		AddGameObeject(mBG, eLayerType::BG);

		mSkul = new Skul();
		AddGameObeject(mSkul, eLayerType::Player);
		
		Camera::SetTarget(mSkul);

		Monster* monster = new Monster();
		AddGameObeject(monster, eLayerType::Monster);


		Scene::Initialize();
	}

	void PlayeScene::Update()
	{
		if (Input::GetKeyState(eKeyCode::N) == eKeyState::Down)
		{
			SceneManager::LoadScene(eSceneType::Title);
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
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Monster, true);

	}
	void PlayeScene::OnExit()
	{
		
		//mCuphead->SetPos(Vector2{ 0.0f, 0.0f });
	}
}
