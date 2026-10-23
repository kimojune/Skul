#include "yaRigidbody.h"
#include "yaTime.h"
#include "yaGameObject.h"
#include "yaTransform.h"

namespace ya
{
	Rigidbody::Rigidbody()
		: Component(eComponentType::Rigidbody)
		, mMass(0.0f)
		, mForce(Vector2::Zero)
		, mAccelation(Vector2::Zero)
		, mVelocity(Vector2::Zero)
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

		Vector2 pos = tr->GetPos();
		pos = pos + mVelocity * Time::DeltaTime();


		tr->SetPos(pos);
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
}