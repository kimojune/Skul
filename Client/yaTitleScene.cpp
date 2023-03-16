#include "yaTitleScene.h"
#include "yaInput.h"
#include "yaSceneManager.h"
#include "yaTitleBG.h"
#include "yaTitleLogo.h"
#include "yaFadeInOut.h"
#include "yaSkul.h"
#include "yaMonster.h"
#include "yaCollisionManager.h"
#include "yaTransform.h"
#include "yaCamera.h"
#include "yaObject.h"

namespace ya
{
	TitleScene::TitleScene()
	{

	}
	TitleScene::~TitleScene()
	{
	}
	void TitleScene::Initialize()
	{

		Scene::Initialize();

		object::Instantiate<TitleBG>(eLayerType::BG);
		//BG* mBG = new BG;
		//mBG->Initialize();
		//AddGameObeject(mBG, eLayerType::BG);

	}
	void TitleScene::Update()
	{
		Scene::Update();

		if (Input::GetKeyState(eKeyCode::N) == eKeyState::Down)
		{
			SceneManager::LoadScene(eSceneType::Play);	
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
		CollisionManager::SetLayer(eLayerType::Player, eLayerType::Monster, true);
	}
	void TitleScene::OnExit()
	{
	}
}
