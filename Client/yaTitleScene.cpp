#include "yaTitleScene.h"
#include "yaInput.h"
#include "yaSceneManager.h"
#include "yaTitleBG.h"
#include "yaTitleLogo.h"
#include "yaSkul.h"
#include "yaMonster.h"
#include "yaCollisionManager.h"
#include "yaTransform.h"
#include "yaCamera.h"
#include "yaObject.h"
#include "yaSound.h"
#include "yaResources.h"

namespace ya
{
	TitleScene::TitleScene()
		:mainTheme(nullptr)
	{

	}
	TitleScene::~TitleScene()
	{
	}
	void TitleScene::Initialize()
	{
		mainTheme = Resources::Load<Sound>(L"MainTheme", L"..\\Resources\\Sound\\TitleTheme.wav");
		//mainTheme->Play(true);


		Scene::Initialize();

		AddGameObeject(mSkuls[(UINT)Skul::eSkulType::Basic], eLayerType::Player);
		AddGameObeject(mSkuls[(UINT)Skul::eSkulType::Nohead], eLayerType::Player);

	

		object::Instantiate<TitleBG>(eLayerType::BG);
		object::Instantiate<TitleLogo>(Vector2(480.0f,550.0f),(eLayerType::Effect));
		//BG* mBG = new BG;
		//mBG->Initialize();
		//AddGameObeject(mBG, eLayerType::BG);

	}
	void TitleScene::Update()
	{
		Scene::Update();

		if (Input::GetKeyState(eKeyCode::N) == eKeyState::Down)
		{
			SceneManager::LoadScene(eSceneType::Stage1);	
		}
	}
	void TitleScene::Render(HDC hdc)
	{
		Scene::Render(hdc);
	}
	void TitleScene::Release()
	{
		Scene::Release();
	}
	void TitleScene::OnEnter()
	{
		mainTheme->Play(true);
		
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Monster, true);
	}
	void TitleScene::OnExit()
	{
		mainTheme->Stop(true);
	}
}
