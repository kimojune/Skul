#include "yaStage1.h"
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
	Stage1::Stage1()
	{
	}
	Stage1::~Stage1()
	{
	}
	void Stage1::Initialize()
	{
		PlayeScene::Initialize();

		mSkuls[(UINT)Skul::eSkulType::Basic] = object::Instantiate<Basic>(Vector2(0.0f, 450.0f), eLayerType::Player);
		mSkuls[(UINT)Skul::eSkulType::Nohead] = object::Instantiate<Nohead>(Vector2(0.0f, 0.0f), eLayerType::Player);


		object::Instantiate<BackCastle>(Vector2(500.0f, 0.0f), eLayerType::Struct);
		object::Instantiate<Castle>(Vector2(0.0f, 0.0f), eLayerType::Struct);
		object::Instantiate<BossGate>(Vector2(1600.0f, 1678.0f), eLayerType::Struct);



		object::Instantiate<Monster>(Vector2(500.0f, 1700.0f), eLayerType::Monster);
		object::Instantiate<Monster>(Vector2(400.0f, 1700.0f), eLayerType::Monster);

		object::Instantiate<Ground>(Vector2(0.0f, 1728.0f), eLayerType::Ground);

		mActiveSkul = mSkuls[(UINT)Skul::eSkulType::Basic];
	}
	void Stage1::Update()
	{
		PlayeScene::Update();

	}
	void Stage1::Render(HDC hdc)
	{
		PlayeScene::Render(hdc);

	}
	void Stage1::Release()
	{
		PlayeScene::Release();

	}
	void Stage1::OnEnter()
	{
		const std::wstring& path = { L"..\\Tile\\stage1" };
		TilePalatte::Load(path);

		PlayeScene::OnEnter();

	}
	void Stage1::OnExit()
	{
		PlayeScene::OnExit();

	}
}
