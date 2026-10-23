#pragma once
#include "yaComponent.h"
#include "yaCollisionMap.h"

namespace ya
{
	class Rigidbody : public Component
	{
	public:
		Rigidbody();
		~Rigidbody();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		void SetMass(float mass) { mMass = mass; }
		void AddForce(Vector2 force);
		void SetGround(bool ground) { mbGround = ground; }
		bool GetGround() { return mbGround; }
		void SetVelocity(Vector2 velocity) { mVelocity = velocity; }
		void Setfriction(float force) { mFriction = force; }
		Vector2 GetVelocity() { return mVelocity; }

		// 발판(OneWay) 위에 서 있으면 떨어뜨린다. 단단한 땅 위에서는 아무 일도 없다. (↓ 입력은 yaPlatform.cpp)
		void DropThrough();
		bool IsOnPlatform() { return mbGround && mbOnPlatform; }

		// 발(콜라이더 아래 변) 바로 아래 줄이 맵의 윗면이면 그 줄, 아니면 CollisionMap::NoSurface
		int GetFootSurface(CollisionMap::ePixel* type);

	private:
		// 이번 스텝에 움직인 뒤 픽셀 충돌 맵으로 착지를 판정한다
		void resolveGround(const Vector2& prevPos);
		void landAt(float footRow, float footOffset, bool platform);
		bool getFoot(int& left, int& right, float& footOffset);

		float mMass;
		Vector2 mForce;
		Vector2 mAccelation;
		Vector2 mVelocity;
		Vector2 mLimitedVelocity;

		Vector2 mGravity;
		float mFriction;
		bool mbGround;

		bool mbOnPlatform;     // 서 있는 곳이 OneWay 발판
		bool mbDropping;       // ↓로 발판에서 내려가는 중
		float mDropFromFoot;   // 내려가기 시작한 발 높이
		float mLastGroundFoot; // 마지막으로 서 있던 발 높이
		bool mbHasLastPos;
		Vector2 mLastPos;      // 지난 스텝이 끝난 위치 (순간이동 감지용)
	};
}
