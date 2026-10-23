#include "yaLayer.h"


namespace ya
{
	Layer::Layer()
	{
	}
	Layer::~Layer()
	{
		for (GameObject* gameObj : mGameObjects)
		{
			delete gameObj;
			gameObj = nullptr;
		}
	}
	void Layer::Initialize()
	{
		for (GameObject* gameObj : mGameObjects)
		{
			if (gameObj->GetState()
				!=GameObject::eState::Active)
				continue;

			gameObj->Initialize();
		}
	}
	void Layer::Update()
	{
		// Update 중에 object::Instantiate로 오브젝트가 추가되면 벡터가 재할당될 수 있어
		// range-for 대신 인덱스로 돈다. 새로 생긴 오브젝트는 다음 프레임부터 Update된다.
		const size_t count = mGameObjects.size();
		for (size_t i = 0; i < count && i < mGameObjects.size(); i++)
		{
			GameObject* gameObj = mGameObjects[i];
			if (gameObj == nullptr)
				continue;

			if (gameObj->GetState()
				!= GameObject::eState::Active)
				continue;

			gameObj->Update();
		}
	}
	void Layer::Render(HDC hdc)
	{
		const size_t count = mGameObjects.size();
		for (size_t i = 0; i < count && i < mGameObjects.size(); i++)
		{
			GameObject* gameObj = mGameObjects[i];
			if (gameObj == nullptr)
				continue;

			if (gameObj->GetState()
				!= GameObject::eState::Active)
				continue;

			gameObj->Render(hdc);
		}
	}
	void Layer::Release()
	{

	}
	void Layer::AddGameObject(GameObject* gameObj)
	{
		if (gameObj == nullptr)
			return;

		mGameObjects.push_back(gameObj);
	}
}
