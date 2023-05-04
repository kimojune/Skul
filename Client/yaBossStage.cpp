#include "yaBossStage.h"
#include "yaSkul.h"
#include "yaInput.h"
#include "yaSceneManager.h"
#include "yaMonster.h"
#include "yaCollisionManager.h"
#include "yaBossBG.h"
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
#include "yaPlatform.h"
#include "yaChapter1_Boss.h"

namespace ya
{
	BossStage::BossStage()
	{
	}
	BossStage::~BossStage()
	{
		Scene* scene = dynamic_cast<Scene*>(this);

		scene->GetGameObjects(eLayerType::Player)[0] = nullptr;
		scene->GetGameObjects(eLayerType::Player)[1] = nullptr;



	}
	void BossStage::Initialize()
	{
		PlayeScene::Initialize();

		

		AddGameObeject(mSkuls[(UINT)Skul::eSkulType::Basic], eLayerType::Player);
		AddGameObeject(mSkuls[(UINT)Skul::eSkulType::Nohead], eLayerType::Player);


		object::Instantiate<Ground>(Vector2(0.0f, 1750.0f), eLayerType::Ground);
		object::Instantiate<PlayBG>(Vector2(0.0f, 0.0f), eLayerType::BG);
		object::Instantiate<BossBG>(Vector2(0.0f, 0.0f), eLayerType::Struct);
		object::Instantiate<Chapter1_Boss>(Vector2(1400.0f,1400.0f), eLayerType::Monster);
		ElderEnt_Roar = Resources::Load<Sound>(L"ElderEnt_Roar", L"..\\Resources\\Sound\\Boss\\ElderEnt_Roar.wav");


	}
	void BossStage::Update()
	{
		PlayeScene::Update();
	}
	void BossStage::Render(HDC hdc)
	{
		PlayeScene::Render(hdc);

	}
	void BossStage::Release()
	{
		PlayeScene::Release();
	}
	void BossStage::OnEnter()
	{
		const std::wstring& path = { L"..\\Tile\\stage1" };
		Chapter1->Stop(true);
		//Chapter1_BossBG->Play(true);
		ElderEnt_Roar->Play(false);
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
		
		

		tr->SetPos(Vector2(1200, 1500));

		PlayeScene::OnEnter();
	}
	void BossStage::OnExit()
	{
		//Chapter1_BossBG->Stop(true);
	}
}