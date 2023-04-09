#include "yaPlayer.h"
#include "yaBasicSkul.h"
#include "yaTransform.h"
#include "yaSceneManager.h"
//#include "yaLayer.h"

namespace ya
{
	Player::Player()
	{
		mSkul.resize((UINT)eSkulType::End);
	}
	Player::~Player()
	{
	}
	void Player::Initialize()
	{
		mSkul[(UINT)eSkulType::Basic] = new BasicSkul;
		/*mSkul[(UINT)eSkulType::NoHead] = new NoheadSkul;
		mSkul[(UINT)eSkulType::Basic] = new BasicSkul;
		mPlayer = object::Instantiate<Player>(Vector2(400.0f, 0.0f), eLayerType::Player);
		mPlayer = object::Instantiate<Player>(Vector2(400.0f, 0.0f), eLayerType::Player);
		mPlayer = object::Instantiate<Player>(Vector2(400.0f, 0.0f), eLayerType::Player);*/

		for (Skul* skul : mSkul)
		{
			if (skul == nullptr)
				continue;

			skul->Initialize();
		}

		mActiveSkul = mSkul[(UINT)eSkulType::Basic];

		GameObject::Initialize();

	}
	void Player::Update()
	{
		mActiveSkul->Update(); 
		Transform* tr = GetComponent<Transform>();
		GameObject::Update();
		tr->SetPos(mActiveSkul->GetComponent<Transform>()->GetPos());

	}
	void Player::Render(HDC hdc)
	{
		mActiveSkul->Render(hdc);
		GameObject::Render(hdc);

	}
	void Player::Release()
	{
		mActiveSkul->Release();
		GameObject::Release();

	}
	//void Player::changeSkul()
	//{
	//	std::vector<Skul*> mSkul;
	//	Skul* mActiveSkul;
	//	// Vector2 prevPos = mActiveSkul->GetComponent<Transform>()->GetPos();

	//	// mActiveSkul = ¹Ù²ð °Í;

	//	// mActiveSkul->GetComponent<Transform>()->SetPos(prevPos);

	//	std::vector<GameObject*> playerLayer = SceneManager::GetActiveScene()->GetGameObjects(eLayerType::Player);

	//	playerLayer[0] = mActiveSkul;
	//} 
}
