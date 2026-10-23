#include "yaSkulHead.h"
#include "yaTransform.h"
#include "yaTime.h"
#include "yaObject.h"
#include "yaCollider.h"
#include "yaCamera.h"
#include "yaResources.h"
#include "yaSkul.h"
#include "yaPlayeScene.h"
#include "yaAnimator.h"
#include "yaMonster.h"
#include "yaNoheadSkul.h"
#include "yaPlayerStats.h"
#include "yaScenario.h"
#include "yaSceneManager.h"
#include "yaScene.h"


namespace ya
{
	// Render는 15x13 이미지를 2.8배로 (x, y - 50)부터 그린다. 그래서 그림의 가운데는 x + 21, 아래 끝은 y - 14
	static constexpr float SkulHeadHalfWidth = 21.0f;
	static constexpr float SkulHeadBottomOffset = 14.0f;
	// 순간이동한 몸을 바닥에 세울 때 Transform y = 바닥 윗면 - 24 (몸 충돌체가 가운데 기준 위아래 25라 1px 걸친다.
	// Ground가 착지시키는 높이와 같다)
	static constexpr float SkulHeadBodyStandOffset = 24.0f;

	SkulHead::SkulHead()
		:mTime(0.0f)
		, mDirect(eDirection::Left)
		, mLeftImage(nullptr)
		, mRightImage(nullptr)
		, mHeadState(eHeadState::Hidden)
		, mVelocity(Vector2::Zero)
		, mFloorY(0.0f)
	{
		SetName(L"SkulHead");

	}
	SkulHead::~SkulHead()
	{
	}
	void SkulHead::Initialize()
	{
		mLeftImage = Resources::Load<Image>(L"LeftSkulHead", L"..\\Resources\\LeftSkulHead.bmp");
		mRightImage = Resources::Load<Image>(L"RightSkulHead", L"..\\Resources\\SkulHead.bmp");
		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(45.0f, 45.0f));
		collider->SetCenter(Vector2(0.0f, -50.0f));
		SetState(eState::Pause);

		GameObject::Initialize();

		// 시나리오는 시작할 때 변수 이름을 검사하므로 초깃값을 미리 내보낸다
		publish();
	}
	void SkulHead::Update()
	{
		float deltaTime = (float)Time::DeltaTime();
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();

		switch (mHeadState)
		{
		case eHeadState::Flying:
		{
			float dir = (mDirect == eDirection::Left) ? -1.0f : 1.0f;
			pos.x += dir * FlySpeed * deltaTime;
			mTime += deltaTime;

			// 맵 왼쪽 끝에 부딪히면 튕겨 나온다 (전에는 x=0에 붙은 채 계속 날았다)
			if (pos.x < 0.0f)
			{
				pos.x = 0.0f;
				bounce(BounceBackSpeed);
			}
			else if (mTime >= FlyTime)
			{
				bounce(dir * BounceForwardSpeed);
			}
			break;
		}
		case eHeadState::Bouncing:
		{
			float prevBottom = pos.y - SkulHeadBottomOffset;
			mVelocity.y += Gravity * deltaTime;
			pos = pos + mVelocity * deltaTime;

			if (pos.x < 0.0f)
			{
				pos.x = 0.0f;
				mVelocity.x = -mVelocity.x;
			}

			float floorY = 0.0f;
			if (mVelocity.y > 0.0f
				&& findFloor(prevBottom, pos.y - SkulHeadBottomOffset, pos.x + SkulHeadHalfWidth, floorY))
			{
				pos.y = floorY + SkulHeadBottomOffset;
				mVelocity = Vector2::Zero;
				mFloorY = floorY;
				mHeadState = eHeadState::Landed;
			}
			break;
		}
		default:
			break;
		}

		mPos = pos;
		tr->SetPos(mPos);

		GameObject::Update();

		// 줍지 않은 채 A 쿨다운이 끝났거나(원작 6초 쿨다운. 전에는 8초 고정), 맵 아래로 떨어졌으면 몸으로 돌아온다
		if (PlayerStats::GetSkillACooldown() <= 0.0f || pos.y > SafetyBottom)
			returnToBody();

		publish();
	}

	void SkulHead::Throw(Vector2 pos, eDirection direct)
	{
		// 전에는 비행 시간을 순간이동으로 회수할 때 지우지 않아서 다음 던지기가 일찍 돌아왔다
		mDirect = direct;
		mHeadState = eHeadState::Flying;
		mTime = 0.0f;
		mVelocity = Vector2::Zero;
		mFloorY = 0.0f;

		mPos = pos;
		GetComponent<Transform>()->SetPos(pos);
		SetState(eState::Active);
		publish();
	}

	void SkulHead::Hide()
	{
		mHeadState = eHeadState::Hidden;
		mTime = 0.0f;
		mVelocity = Vector2::Zero;
		SetState(eState::Pause);
		publish();
	}

	Vector2 SkulHead::GetTeleportPos()
	{
		Vector2 pos = mPos;
		pos.x += SkulHeadHalfWidth;
		if (mHeadState == eHeadState::Landed)
			pos.y = mFloorY - SkulHeadBodyStandOffset;
		return pos;
	}

	void SkulHead::bounce(float speedX)
	{
		mHeadState = eHeadState::Bouncing;
		mVelocity = Vector2(speedX, -BounceSpeedY);
	}

	bool SkulHead::findFloor(float prevBottom, float bottom, float centerX, float& floorY)
	{
		// 바닥·발판 = Ground 레이어 충돌체의 윗면. 이번 프레임에 윗면을 위에서 지나 내려왔으면 그 위에 놓인다.
		// (머리는 Rigidbody가 없어서 Ground·Platform의 착지 처리를 받지 않고 여기서 직접 찾는다.
		//  바닥 판정이 픽셀 지형으로 바뀌면 이 함수만 그쪽 질의로 바꾸면 된다)
		bool found = false;
		Scene* scene = SceneManager::GetActiveScene();
		for (GameObject* gameObj : scene->GetGameObjects(eLayerType::Ground))
		{
			if (gameObj == nullptr || gameObj->GetState() != eState::Active)
				continue;

			Collider* collider = gameObj->GetComponent<Collider>();
			if (collider == nullptr)
				continue;

			Vector2 colliderPos = collider->GetPos();
			Vector2 colliderSize = collider->GetSize();
			if (centerX < colliderPos.x || colliderPos.x + colliderSize.x < centerX)
				continue;

			float top = colliderPos.y;
			if (prevBottom <= top && top <= bottom && (!found || top < floorY))
			{
				floorY = top;
				found = true;
			}
		}
		return found;
	}

	void SkulHead::tryPickUp(Collider* other)
	{
		if (!IsPickable())
			return;

		// 머리 없는 몸이 닿으면 머리를 회수한다 (원작: 회수하면 A 쿨다운 초기화)
		Nohead* nohead = dynamic_cast<Nohead*>(other->GetOwner());
		if (nohead == nullptr || !nohead->CanTakeHeadBack())
			return;

		nohead->TakeHeadBack(true, true);
	}

	void SkulHead::returnToBody()
	{
		Nohead* nohead = dynamic_cast<Nohead*>(mSkuls[(UINT)Skul::eSkulType::Nohead]);
		if (nohead != nullptr && nohead->GetState() == eState::Active)
		{
			// 대시·순간이동 중이면 끝난 뒤(다음 프레임들)에 돌아온다
			if (nohead->CanTakeHeadBack())
				nohead->TakeHeadBack(false, true);
			return;
		}

		// 조작 중인 머리 없는 몸이 없으면 머리만 숨긴다
		Hide();
		PlayerStats::OnHeadRetrieved(false);
	}

	void SkulHead::publish()
	{
		Scenario::Publish("head.state", (double)mHeadState);
		Scenario::Publish("head.x", mPos.x);
		Scenario::Publish("head.y", mPos.y);
	}

	void SkulHead::Render(HDC hdc)
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = Camera::CaluatePos(tr->GetPos());

		switch (mDirect)
		{
		case eDirection::Left:
			TransparentBlt(hdc
				, pos.x, pos.y - 50.0f
				, mLeftImage->GetWidth() * 2.8f, mLeftImage->GetHeight() * 2.8f
				, mLeftImage->GetHdc()
				, 0, 0
				, mLeftImage->GetWidth(), mLeftImage->GetHeight()
				, RGB(255, 0, 255));
			break;

		case eDirection::Right:
			TransparentBlt(hdc
				, pos.x, pos.y - 50.0f
				, mRightImage->GetWidth() * 2.8f, mRightImage->GetHeight() * 2.8f
				, mRightImage->GetHdc()
				, 0, 0
				, mRightImage->GetWidth(), mRightImage->GetHeight()
				, RGB(255, 0, 255));
			break;

		default:
			break;
		}



		GameObject::Render(hdc);
	}

	void SkulHead::Release()
	{
	}

	void SkulHead::OnCollisionEnter(Collider* other)
	{
		Monster* monster = dynamic_cast<Monster*>(other->GetOwner());

		if (monster == nullptr)
		{
			tryPickUp(other);
			return;
		}

		// 날아가는 동안만 때리고, 맞히면 뒤로 튀어 오른다 (원작: 무언가에 부딪히면 위로 튐).
		// 바닥에 놓인 머리는 몬스터가 지나가도 때리지 않는다
		if (mHeadState != eHeadState::Flying)
			return;

		monster->SetMonsterState(Monster::eMonsterState::Hit);
		bounce(mDirect == eDirection::Left ? BounceBackSpeed : -BounceBackSpeed);
	}

	void SkulHead::OnCollisionStay(Collider* other)
	{
		// 이미 겹친 채로 주울 수 있게 된 경우(떨어진 머리 위에 서 있을 때)도 줍는다
		tryPickUp(other);
	}

	void SkulHead::OnCollisionExit(Collider* other)
	{
	}

}