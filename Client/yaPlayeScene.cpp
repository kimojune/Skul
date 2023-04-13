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

		
		Chapter1 = Resources::Load<Sound>(L"Chapter1", L"..\\Resources\\Sound\\Chapter1.wav");
		//Chapter1->Play(true);
		
		object::Instantiate<PlayBG>(eLayerType::BG);
		mSkuls[(UINT)Skul::eSkulType::Basic] = object::Instantiate<Basic>(Vector2(400.0f, 0.0f), eLayerType::Player);
		mSkuls[(UINT)Skul::eSkulType::Nohead] = object::Instantiate<Nohead>(Vector2(400.0f, 0.0f), eLayerType::Player);



		object::Instantiate<Monster>(Vector2(500.0f, 800.0f), eLayerType::Monster);
		object::Instantiate<Monster>(Vector2(400.0f, 800.0f), eLayerType::Monster);
		object::Instantiate<Ground>(Vector2(-100.0f, 800.0f), eLayerType::Ground);


		mActiveSkul = mSkuls[(UINT)Skul::eSkulType::Basic];
		

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

		Camera::SetTarget(mActiveSkul);
		//Chapter1->Play(true);

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
	void PlayeScene::SetSkul(Skul::eSkulType type)
	{
		mActiveSkul = mSkuls[(UINT)type];
	}
}
