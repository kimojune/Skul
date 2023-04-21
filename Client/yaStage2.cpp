#include "yaStage2.h"
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
	Stage2::Stage2()
	{
	}
	Stage2::~Stage2()
	{
	}
	void Stage2::Initialize()
	{
		PlayeScene::Initialize();



		AddGameObeject(mSkuls[(UINT)Skul::eSkulType::Basic], eLayerType::Player);
		AddGameObeject(mSkuls[(UINT)Skul::eSkulType::Nohead], eLayerType::Player);
		object::Instantiate<Ground>(Vector2(0.0f, 1728.0f), eLayerType::Ground);
		object::Instantiate<ItemGate>(Vector2(2000.0f, 1578.0f), eLayerType::Struct);


	}
	void Stage2::Update()
	{
		PlayeScene::Update();

	}
	void Stage2::Render(HDC hdc)
	{
		PlayeScene::Render(hdc);

	}
	void Stage2::Release()
	{
		//GetGameObjects(eLayerType::Player)[0] = nullptr;
		//GetGameObjects(eLayerType::Player)[1] = nullptr;
		PlayeScene::Release();

	}
	void Stage2::OnEnter()
	{
		const std::wstring& path2 = { L"..\\Tile\\stage2" };
		const std::wstring& path = { L"..\\Tile\\stage1" };
		TilePalatte::Load(path2);
		TilePalatte::Load(path);
		
		for (Skul* skul : mSkuls)
		{	
			if (skul->GetState() == GameObject::eState::Active)
				mActiveSkul = skul;
			else
				continue;
		}
		if (mActiveSkul == nullptr)
			return;

		Transform* tr = mActiveSkul->GetComponent<Transform>();

		tr->SetPos(Vector2(0, 1500));

		PlayeScene::OnEnter();

	}
	void Stage2::OnExit()
	{
		PlayeScene::OnExit();

	}
}