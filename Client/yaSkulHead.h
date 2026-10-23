#pragma once
#include "yaGameObject.h"
#include "yaImage.h"

namespace ya
{
	class Skul;

	// A로 던진 리틀 본의 머리. 원작처럼 앞으로 날아가다가 무언가(몬스터, 맵 왼쪽 끝)에 부딪히거나
	// 일정 시간이 지나면 위로 튀어 떨어지고, 바닥에 놓여 주울 수 있는 상태로 남는다.
	//  - 머리 없는 몸(Nohead)이 닿으면 머리를 되찾고 A 쿨다운이 초기화된다
	//  - S(순간이동)로 머리 위치에 가면 회수된다 (Nohead::SkillS)
	//  - 줍지 않으면 A 쿨다운(6초)이 끝날 때 몸으로 돌아온다
	//  - 맵 아래로 떨어지면 바로 돌아온다 (안전장치)
	class SkulHead : public GameObject
	{
	public:
		// 시나리오 head.state 값과 같다
		enum class eHeadState
		{
			Hidden,		// 몸에 붙어 있음 (오브젝트는 Pause)
			Flying,		// 앞으로 날아가는 중. 이때만 몬스터를 때린다
			Bouncing,	// 튀어 올랐다 떨어지는 중
			Landed,		// 바닥에 놓여 있음
		};

		// 원작 수치를 찾지 못해(불확실) 이 클론에서 정한 값
		static constexpr float FlySpeed = 1060.0f;		// 기존 속도 그대로 (1500 x cos45)
		static constexpr float FlyTime = 0.6f;			// 이만큼 날면 튀어 오른다 (약 640px)
		static constexpr float BounceSpeedY = 420.0f;	// 튀어 오르는 속도 (약 50px 높이)
		static constexpr float BounceForwardSpeed = 150.0f;	// 날다가 튈 때 앞으로 남는 속도
		static constexpr float BounceBackSpeed = 200.0f;	// 부딪혀 튈 때 뒤로 밀리는 속도
		static constexpr float Gravity = 1800.0f;
		static constexpr float SafetyBottom = 2400.0f;	// 이 아래로 떨어지면 맵 밖 (바닥 충돌체 아래 끝 2250)

		SkulHead();
		~SkulHead();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		virtual void OnCollisionEnter(class Collider* other);
		virtual void OnCollisionStay(class Collider* other);
		virtual void OnCollisionExit(class Collider* other);

		Vector2 GetPos() { return mPos; }

		virtual void SetDirect(eDirection direct) { mDirect = direct; }

		// pos(던진 스컬의 위치)에서 direct 방향으로 던진다. 이전 비행 상태는 모두 지운다
		void Throw(Vector2 pos, eDirection direct);
		// 몸에 붙인다 (Pause). PlayerStats의 머리 상태는 부르는 쪽이 바꾼다
		void Hide();
		eHeadState GetHeadState() { return mHeadState; }
		// 몸이 닿으면 주울 수 있는가 (다 날아간 뒤부터)
		bool IsPickable() { return mHeadState == eHeadState::Bouncing || mHeadState == eHeadState::Landed; }
		// 순간이동한 몸이 설 위치. 바닥에 놓인 머리면 그 바닥 위, 공중이면 머리 높이(던진 높이 기준)
		Vector2 GetTeleportPos();

	private:
		void bounce(float speedX);
		bool findFloor(float prevBottom, float bottom, float centerX, float& floorY);
		void tryPickUp(Collider* other);
		void returnToBody();
		void publish();

		float mTime;
		Image* mLeftImage;
		Image* mRightImage;
		Vector2 mPos;
		eDirection mDirect;

		eHeadState mHeadState;
		Vector2 mVelocity;	// 튀는 동안의 속도
		float mFloorY;		// 놓인 바닥의 윗면 y
	};
}
