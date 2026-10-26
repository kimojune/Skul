#include "yaRigidbody.h"
#include "yaTime.h"
#include "yaGameObject.h"
#include "yaTransform.h"
#include "yaCollider.h"
#include <cfloat>

namespace ya
{
	// 픽셀 지형 판정 값 (px)
	static const int RigidbodyStepUp = 10;        // 서 있을 때 이만큼 높은 윗면까지는 올라선다 (풀잎 요철, 스컬 교대 때 발 높이 차)
	static const int RigidbodySnapDown = 12;      // 서 있을 때 이만큼 낮은 윗면까지는 붙어서 내려간다. 더 깊으면 떨어진다
	static const int RigidbodyPushOut = 128;      // 생성·순간이동으로 땅 속에 놓였을 때 위로 밀어 올리는 최대 거리
	static const int RigidbodyDropDepth = 80;     // ↓로 내려갈 때 이 깊이까지는 발판을 무시 (발판 그림 두께는 타일 72 이하)
	static const float RigidbodyTeleport = 32.0f; // 스텝 사이에 이보다 멀리 옮겨졌으면 순간이동으로 보고 새로 착지 판정

	Rigidbody::Rigidbody()
		: Component(eComponentType::Rigidbody)
		, mMass(0.0f)
		, mForce(Vector2::Zero)
		, mAccelation(Vector2::Zero)
		, mVelocity(Vector2::Zero)
		, mbOnPlatform(false)
		, mbDropping(false)
		, mDropFromFoot(0.0f)
		, mLastGroundFoot(FLT_MAX)
		, mbHasLastPos(false)
		, mLastPos(Vector2::Zero)
	{
		mLimitedVelocity.x = 100000.0f;
		mLimitedVelocity.y = 1000.0f;
		mbGround = false;
		mGravity = Vector2(0.0f, 800.0f);
		mFriction = 100.0f;
	}

	Rigidbody::~Rigidbody()
	{

	}

	void Rigidbody::Initialize()
	{

	}

	void Rigidbody::Update()
	{
		// F = M * A
		// A = M / F
		mAccelation = mForce / mMass;

		// 속도에 가속도를 더해준다.
		//mVelocity += mAccelation * Time::DeltaTime();

		if (mbGround)
		{
			Vector2 gravity = mGravity;
			gravity.Normalize();

			float dot = math::Dot(mVelocity, gravity);
			mVelocity -= gravity * dot;

		}

		else
		{
			mVelocity += mGravity * Time::DeltaTime();
		}

		// 최대 속도 제한: 속도를 중력 방향 성분과 옆 성분으로 나눠서 각각 제한한다
		Vector2 gravityDir = mGravity;
		gravityDir.Normalize();
		float fallSpeed = math::Dot(mVelocity, gravityDir);
		Vector2 sideVelocity = mVelocity - gravityDir * fallSpeed;

		// 중력 방향은 떨어지는 속도만 제한한다 (점프로 올라가는 속도는 그대로)
		if (mLimitedVelocity.y < fallSpeed)
			fallSpeed = mLimitedVelocity.y;

		if (mLimitedVelocity.x < sideVelocity.Length())
		{
			sideVelocity.Normalize();
			sideVelocity *= mLimitedVelocity.x;
		}

		// 마찰력은 옆 성분에만 적용한다 (낙하·점프 속도는 줄이지 않는다)
		float sideSpeed = sideVelocity.Length();
		if (sideSpeed > 0.0f)
		{
			float friction = mFriction * mMass * (float)Time::DeltaTime();

			// 마찰력으로 인한 속도 감소가 현재 속도보다 크면 멈춘다
			if (sideSpeed <= friction)
			{
				sideVelocity = Vector2::Zero;
			}
			else
			{
				sideVelocity.Normalize();
				sideVelocity *= sideSpeed - friction;
			}
		}

		mVelocity = gravityDir * fallSpeed + sideVelocity;

		// 속도에 맞게끔 물체를 이동시킨다.
		Transform* tr = GetOwner()->GetComponent<Transform>();

		Vector2 prevPos = tr->GetPos();
		Vector2 pos = prevPos + mVelocity * Time::DeltaTime();


		tr->SetPos(pos);

		// 움직인 뒤 발밑을 픽셀 충돌 맵으로 판정한다
		resolveGround(prevPos);

		mLastPos = tr->GetPos();
		mbHasLastPos = true;
		mForce.Clear();
	}

	void Rigidbody::Render(HDC hdc)
	{
	}
	void Rigidbody::Release()
	{
	}
	void Rigidbody::AddForce(Vector2 force)
	{
		mForce += force;
	}

	void Rigidbody::DropThrough()
	{
		if (!mbGround || !mbOnPlatform)
			return;

		int left = 0;
		int right = 0;
		float footOffset = 0.0f;
		if (!getFoot(left, right, footOffset))
			return;

		mDropFromFoot = GetOwner()->GetComponent<Transform>()->GetPos().y + footOffset;
		mbDropping = true;
		mbGround = false;
		mbOnPlatform = false;
	}

	int Rigidbody::GetFootSurface(CollisionMap::ePixel* type)
	{
		int left = 0;
		int right = 0;
		float footOffset = 0.0f;
		if (!CollisionMap::IsReady() || !getFoot(left, right, footOffset))
			return CollisionMap::NoSurface;

		const int footRow = (int)roundf(GetOwner()->GetComponent<Transform>()->GetPos().y + footOffset);
		return CollisionMap::FindSurface(left, right, footRow, footRow, footRow, type);
	}

	// 발 = 콜라이더 아래 변. [left, right) 열, 아래 변 높이 = 위치.y + footOffset
	bool Rigidbody::getFoot(int& left, int& right, float& footOffset)
	{
		Collider* collider = GetOwner()->GetComponent<Collider>();
		if (collider == nullptr)
			return false;

		// 콜라이더 위치를 지금 Transform에 맞춘다 (Collider::Update = 위치 + center). 이 프레임 끝에서도 다시 갱신된다
		collider->Update();

		Vector2 pos = GetOwner()->GetComponent<Transform>()->GetPos();
		Vector2 leftTop = collider->GetPos();
		Vector2 size = collider->GetSize();

		left = (int)floorf(leftTop.x);
		right = left + max(1, (int)size.x);
		// (위치 + center) - 위치 의 부동소수 오차를 없앤다 (center는 보통 정수라 착지 위치가 정수로 떨어진다)
		footOffset = roundf((leftTop.y - pos.y + size.y) * 16.0f) / 16.0f;
		return true;
	}

	void Rigidbody::landAt(float footRow, float footOffset, bool platform)
	{
		Transform* tr = GetOwner()->GetComponent<Transform>();
		Vector2 pos = tr->GetPos();
		pos.y = footRow - footOffset;
		tr->SetPos(pos);

		if (mVelocity.y > 0.0f)
			mVelocity.y = 0.0f;

		mbGround = true;
		mbOnPlatform = platform;
		mbDropping = false;
		mLastGroundFoot = footRow;
	}

	// 픽셀 충돌 맵 착지 판정 (CollisionMap: Solid = 마젠타 땅, OneWay = 발판)
	//  - 발밑 줄이 땅이 아니면 착지가 아니다 → 떨어진다
	//  - 이번 스텝에 발이 지나간 줄을 모두 훑어 처음 닿는 윗면에 발을 맞추고 낙하 속도를 0으로 한다
	//  - 발판은 떨어지는 중이고 이전 스텝에 발이 그 윗면보다 위(같은 높이 포함)였을 때만 잡는다
	void Rigidbody::resolveGround(const Vector2& prevPos)
	{
		if (!CollisionMap::IsReady())
			return;

		int left = 0;
		int right = 0;
		float footOffset = 0.0f;
		if (!getFoot(left, right, footOffset))
			return;

		Transform* tr = GetOwner()->GetComponent<Transform>();
		const float prevFoot = prevPos.y + footOffset;
		const float foot = tr->GetPos().y + footOffset;
		const int bodyBottomRow = (int)ceilf(foot) - 1; // 몸의 맨 아래 픽셀 줄

		// 지난 스텝 뒤에 멀리 옮겨졌으면 순간이동(스컬 교대·머리 위치 이동·씬 시작)으로 본다
		const bool wasGround = mbGround;
		Vector2 moved = Vector2(prevPos.x - mLastPos.x, prevPos.y - mLastPos.y);
		const bool teleported = mbHasLastPos && moved.Length() > RigidbodyTeleport;
		if (teleported)
			mbDropping = false;

		// 올라가는 중: 땅에 붙지 않는다 (발판도, 천장 같은 땅도 아래에서 그대로 통과)
		if (mVelocity.y < 0.0f)
		{
			mbGround = false;
			mbOnPlatform = false;
			return;
		}

		// 1) 이번 스텝에 발이 지나간 줄을 위에서부터 훑어 처음 닿는 윗면 (빠르게 떨어져도 뚫지 않는다)
		int fromRow = (int)ceilf(prevFoot);
		int toRow = (int)floorf(foot);
		int oneWayFrom = fromRow; // 발판은 이전 발 높이보다 위에 있는 윗면을 잡지 않는다
		if (wasGround)
		{
			// 서 있으면 풀잎 요철을 따라 조금 오르내린다
			fromRow -= RigidbodyStepUp;
			toRow += RigidbodySnapDown;
			oneWayFrom = fromRow;
		}
		if (mbDropping)
			oneWayFrom = max(oneWayFrom, (int)ceilf(mDropFromFoot) + RigidbodyDropDepth);

		CollisionMap::ePixel type = CollisionMap::ePixel::Empty;
		int surface = CollisionMap::FindSurface(left, right, fromRow, toRow, oneWayFrom, &type);
		if (surface != CollisionMap::NoSurface)
		{
			landAt((float)surface, footOffset, type == CollisionMap::ePixel::OneWay);
			return;
		}

		if (mbDropping && foot > mDropFromFoot + RigidbodyDropDepth)
			mbDropping = false;

		// 2) 닿은 윗면이 없고 발이 땅 속도 아니면 떨어진다 (난간에서 걸어 나간 경우 포함)
		if (!CollisionMap::IsSolidRow(left, right, bodyBottomRow))
		{
			mbGround = false;
			mbOnPlatform = false;
			return;
		}

		// 3) 발이 땅 속에 있다
		if (wasGround && !teleported)
		{
			// 서 있다가 벽 같은 땅 속으로 옆걸음해 들어갔다: 발밑이 땅이므로 그 높이에 서 있는다
			landAt(roundf(foot), footOffset, false);
			return;
		}

		if (!teleported && mbHasLastPos && foot < mLastGroundFoot)
		{
			// 공중에서 땅 속으로 들어갔다 (올라가다 천장으로, 떨어지다 벽 옆면으로):
			// 빠져나올 때까지 그대로 떨어진다. 땅이 아래까지 이어져 있어도 마지막으로 서 있던 높이에서는 멈춘다
			mbGround = false;
			mbOnPlatform = false;
			return;
		}

		if (!teleported && mbHasLastPos && prevFoot <= mLastGroundFoot)
		{
			// 이번 스텝에 마지막으로 서 있던 높이를 지났다
			landAt(mLastGroundFoot, footOffset, false);
			return;
		}

		// 생성·순간이동으로 땅 속에 놓였거나, 서 있던 높이보다 아래에서 땅 속으로 들어갔다:
		// 가장 가까운 윗면으로 밀어 올린다 (가장 적게 움직이는 곳). 너무 깊으면 발밑이 땅이므로 그 높이에 서 있는다
		const int footRow = (int)ceilf(foot);
		surface = CollisionMap::FindSolidSurfaceAbove(left, right, footRow - 1, footRow - RigidbodyPushOut);
		landAt(surface != CollisionMap::NoSurface ? (float)surface : roundf(foot), footOffset, false);
	}
}