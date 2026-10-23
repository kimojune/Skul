#include "yaGround.h"
#include "yaCollider.h"
#include "yaSkul.h"
#include "yaRigidbody.h"
#include "yaTransform.h"
#include "yaResources.h"
#include "yaImage.h"
#include "yaPlayeScene.h"
#include "yaSceneManager.h"
#include "yaCamera.h"
#include "yaTilePalatte.h"
#include "yaCollisionMap.h"
#include "yaScenario.h"

namespace ya
{
	Ground::Ground()
	{
	}

	Ground::~Ground()
	{
	}

	// 시나리오 상태 변수: 픽셀 충돌 맵 기준으로 몬스터·플레이어가 땅에 붙어 있는지 (Scenarios/README.md)
	static void GroundPublishMapState()
	{
		int monsters = 0;
		int monstersGround = 0;
		int monstersTouch = 0;
		int playerTouch = 0;
		int playerUnder = 0;
		Vector2 monsterPos[3] = {}; // 앞의 몬스터 3마리 위치 (발 높이 측정용)

		Scene* scene = SceneManager::GetActiveScene();
		if (scene != nullptr && CollisionMap::IsReady())
		{
			for (GameObject* obj : scene->GetGameObjects(eLayerType::Monster))
			{
				if (obj == nullptr || obj->GetState() != GameObject::eState::Active)
					continue;

				Rigidbody* rb = obj->GetComponent<Rigidbody>();
				Collider* col = obj->GetComponent<Collider>();
				if (rb == nullptr || col == nullptr)
					continue;

				if (monsters < 3)
					monsterPos[monsters] = obj->GetComponent<Transform>()->GetPos();

				monsters++;
				if (!rb->GetGround())
					continue;

				monstersGround++;
				if (rb->GetFootSurface(nullptr) != CollisionMap::NoSurface)
					monstersTouch++;
			}

			PlayeScene* playScene = dynamic_cast<PlayeScene*>(scene);
			Skul* skul = (playScene != nullptr) ? playScene->GetSkul() : nullptr;
			Rigidbody* rb = (skul != nullptr) ? skul->GetComponent<Rigidbody>() : nullptr;
			if (rb != nullptr && rb->GetGround())
			{
				CollisionMap::ePixel type = CollisionMap::ePixel::Empty;
				if (rb->GetFootSurface(&type) != CollisionMap::NoSurface)
				{
					playerTouch = 1;
					playerUnder = (int)type;
				}
			}
		}

		Scenario::Publish("map.monsters", monsters);              // 리지드바디가 있는 활성 몬스터 수
		Scenario::Publish("map.monsters_ground", monstersGround); // 그중 착지 상태인 수
		Scenario::Publish("map.monsters_touch", monstersTouch);   // 그중 발(콜라이더 아래 변)이 맵의 윗면 줄에 딱 붙은 수
		Scenario::Publish("map.player_touch", playerTouch);       // 플레이어 발이 윗면 줄에 딱 붙어 있으면 1
		Scenario::Publish("map.player_under", playerUnder);       // 발밑: 0 없음, 1 단단한 땅, 2 발판
		Scenario::Publish("map.m0.x", monsterPos[0].x);
		Scenario::Publish("map.m0.y", monsterPos[0].y);
		Scenario::Publish("map.m1.x", monsterPos[1].x);
		Scenario::Publish("map.m1.y", monsterPos[1].y);
		Scenario::Publish("map.m2.x", monsterPos[2].x);
		Scenario::Publish("map.m2.y", monsterPos[2].y);
	}

	void Ground::Initialize()
	{
		// 지면 충돌은 타일 그림으로 만든 픽셀 충돌 맵으로 한다 (yaCollisionMap, Rigidbody).
		// 이 오브젝트는 씬 배치를 그대로 두기 위해 남겨 두고, 충돌 콜백에서는 아무것도 하지 않는다.
		mCollider = AddComponent<Collider>();
		mCollider->SetSize(Vector2(3400.0f, 500.0f));

		// 대본이 시작할 때 변수 이름을 검사하므로 초깃값을 내보내 둔다
		GroundPublishMapState();

		GameObject::Initialize();
	}

	void Ground::Update()
	{
		GameObject::Update();

		// Ground 레이어는 Monster·Player 레이어 다음에 갱신되므로 이번 프레임의 착지 결과를 본다
		GroundPublishMapState();
	}

	void Ground::Render(HDC hdc)
	{
		Transform* tr = GetComponent<Transform>();
		
		Vector2 renderPos = Camera::CaluatePos(tr->GetPos());


		GameObject::Render(hdc);


		//TransparentBlt(hdc, tr->GetPos().x, tr->GetPos().y
		//	, mImage->GetWidth(), mImage->GetHeight()
		//	, mImage->GetHdc()
		//	, 0, 0
		//	, mImage->GetWidth(), mImage->GetHeight()
		//	, RGB(255, 255, 255));

	}

	void Ground::Release()
	{
	}


	void Ground::OnCollisionEnter(Collider* other)
	{
		// 예전에는 여기서 보이지 않는 상자(y 1750) 위로 위치를 옮겼다.
		// 이제 착지는 Rigidbody가 픽셀 충돌 맵으로 하므로 아무것도 하지 않는다.
	}
	void Ground::OnCollisionStay(Collider* other)
	{

	}
	void Ground::OnCollisionExit(Collider* other)
	{
		//Skul* skul = dynamic_cast<Skul*>(other->GetOwner());


		//Rigidbody* rb = other->GetOwner()->GetComponent<Rigidbody>();
		////Rigidbody* rb = skul->GetComponent<Rigidbody>();
		//if (rb == nullptr)
		//	return;
		//rb->SetGround(false);

	}
}