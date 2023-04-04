#include "yaSceneManager.h"
#include "yaPlayeScene.h"
#include "yaTitleScene.h"
#include "yaToolScene.h"
#include "yaCollisionManager.h"
#include "yaCamera.h"

namespace ya
{	
	//SceneManager scsene;
	//SceneManager* scsene = new SceneManager();
	std::vector<Scene*> SceneManager::mScenes = {};
	Scene* SceneManager::mActiveScene = nullptr;

	void SceneManager::Initialize()
	{
		mScenes.resize((UINT)eSceneType::End);

		mScenes[(UINT)eSceneType::Title] = new TitleScene();
		mScenes[(UINT)eSceneType::Play] = new PlayeScene();
		mScenes[(UINT)eSceneType::Tool] = new ToolScene();


		for ( Scene* scene : mScenes )
		{
			if (scene == nullptr)
				continue;

			scene->Initialize();
		}

		mActiveScene = mScenes[(UINT)eSceneType::Tool];

	}

	void SceneManager::Update()
	{
		mActiveScene->Update();
	}

	void SceneManager::Render(HDC hdc)
	{
		mActiveScene->Render(hdc);
	}

	void SceneManager::Release()
	{
		for (Scene* scene : mScenes)
		{
			if (scene == nullptr)
				continue;

			delete scene;
			scene = nullptr;
		}
	}

	void SceneManager::Destroy()
	{
		mActiveScene->Destroy();
	}

	void SceneManager::LoadScene(eSceneType type)
	{
		Camera::Clear();
		// �����
		mActiveScene->OnExit();
		CollisionManager::Clear();
		//������
		mActiveScene = mScenes[(UINT)type];
		mActiveScene->OnEnter();


	}
}
