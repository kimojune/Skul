#include "yaTitleScene.h"
#include "yaInput.h"
#include "yaSceneManager.h"
#include "yaBG.h"
#include "yaTitleLogo.h"
#include "yaFadeInOut.h"

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
		
		BG* mBG = new BG(eSceneType::Title);
		AddGameObeject(mBG, eLayerType::BG);
	
		FadeInOut* mfadeInout = new FadeInOut;
		AddGameObeject(mfadeInout, eLayerType::Effect);

		/*TitleLogo* Logo = new TitleLogo;
		AddGameObeject(Logo, eLayerType::Effect);*/

		Scene::Initialize();
	}
	void TitleScene::Update()
	{
		if (Input::GetKeyState(eKeyCode::N) == eKeyState::Down)
		{
			SceneManager::LoadScene(eSceneType::Play);
			
		}
		Scene::Update();
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
	}
	void TitleScene::OnExit()
	{
	}
}
