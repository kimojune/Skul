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
		
		object::Instantiate<PlayBG>(Vector2(0.0f, 0.0f), eLayerType::BG);

		for (Skul* skul : mSkuls)
			skul->SetState(GameObject::eState::Pause);



		object::Instantiate<BackCastle>(Vector2(500.0f, 0.0f), eLayerType::Struct);
		object::Instantiate<Castle>(Vector2(0.0f, 0.0f), eLayerType::Struct);
		object::Instantiate<ItemGate>(Vector2(1700.0f, 1578.0f), eLayerType::Gate);
		//object::Instantiate<BossGate>(Vector2(2500.0f, 1598.0f), eLayerType::Gate);

		// 엔트 3마리. 모두 쓰러뜨려야 ItemGate 가 열린다 (원작: 방의 적을 다 잡아야 문이 열림).
		// 시작 위치(x=0)에서 감지 거리(500) 밖, 문 앞에 둘, 문 뒤에 하나
		object::Instantiate<Monster>(Vector2(800.0f, 1700.0f), eLayerType::Monster);
		object::Instantiate<Monster>(Vector2(1250.0f, 1700.0f), eLayerType::Monster);
		object::Instantiate<Monster>(Vector2(2400.0f, 1700.0f), eLayerType::Monster);




		object::Instantiate<Ground>(Vector2(0.0f, 1750.0f), eLayerType::Ground);
		
		mActiveSkul = mSkuls[(UINT)Skul::eSkulType::Basic];
		mActiveSkul->SetState(GameObject::eState::Active);
	}
	void Stage1::Update()
	{
		if (Input::GetKeyState(eKeyCode::N) == eKeyState::Down)
		{
			SceneManager::LoadScene(eSceneType::Stage2);
		}
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
		// 카메라 경계: 오른쪽 3350은 기존 값, 아래 2250은 바닥 충돌체 아래 끝 (1750 + 500)
		Camera::SetBounds(0.0f, 0.0f, 3350.0f, 2250.0f);

		const std::wstring& path = { L"..\\Tile\\stage1" };
		TilePalatte::Load(path);
		Chapter1->Play(true);

		PlayeScene::OnEnter();

	}
	void Stage1::OnExit()
	{

		PlayeScene::OnExit();
	}
}
