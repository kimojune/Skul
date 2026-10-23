#pragma once

#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class Image;
	class Collider;
	class Chapter1_Boss;

	// 보스의 손 하나. 왼손은 Boss_Hand, 오른손은 Boss_RightHand(같은 코드에 방향만 다름).
	// 스스로 패턴을 고르지 않고 Chapter1_Boss가 시키는 대로 움직인다.
	// 위치(GetPos/MoveTo)는 손 그림의 "아래쪽 가운데"다. 바닥에 닿게 하려면 y에 바닥 높이를 주면 된다.
	class Boss_Hand : public GameObject
	{
	public:
		enum class ePose
		{
			Hover,	// 주먹 쥔 채 떠 있음 (대기 애니메이션)
			Fist,	// 내려치는 주먹 (Punch)
			Flat,	// 손바닥으로 바닥을 짚음 (PutOn)
			Sweep,	// 바닥을 쓰는 손 (Smesh)
		};

		Boss_Hand();
		Boss_Hand(bool isRight);
		virtual ~Boss_Hand();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		virtual void OnCollisionEnter(class Collider* other) override;
		virtual void OnCollisionStay(class Collider* other) override;
		virtual void OnCollisionExit(class Collider* other) override;

		void SetBoss(Chapter1_Boss* boss) { mBoss = boss; }
		bool IsRight() { return mbRight; }

		void SetPose(ePose pose);
		ePose GetPose() { return mPose; }
		// true면 2페이즈 보라색 그림(_2)을 쓴다. 보라색 그림이 없는 포즈는 원래 그림 그대로
		void SetPhase2(bool phase2);

		// 위치를 speed(px/초)로 옮긴다. 도착하면 IsArrived()가 true
		void MoveTo(Vector2 pos, float speed);
		void Teleport(Vector2 pos);
		bool IsArrived() { return mbArrived; }
		Vector2 GetPos() { return mPos; }

		// 예고 동작: 제자리에서 떨기 (그리는 위치만 흔든다)
		void SetTremble(bool tremble) { mbTremble = tremble; }

		// damage > 0이면 플레이어와 닿을 때 한 번 피해를 준다. 0이면 닿아도 아무 일 없음.
		// 다시 부르면 "이번 공격에 이미 맞힘" 표시가 지워진다.
		// linked: 같은 공격을 함께 하는 다른 손 (양손 내려치기). 한쪽이 맞히면 다른 쪽은 맞히지 않는다
		void SetAttack(int damage, Boss_Hand* linked = nullptr);

	private:
		void playPose();
		void updateCollider();

	private:
		Chapter1_Boss* mBoss;
		Animator* mAnimator;
		Collider* mCollider;
		bool mbRight;
		bool mbPhase2;
		ePose mPose;

		Vector2 mPos;			// 논리 위치 (아래쪽 가운데)
		Vector2 mTarget;
		float mSpeed;
		bool mbArrived;
		bool mbTremble;
		float mTime;

		int mDamage;
		bool mbHitDone;
		Boss_Hand* mLinked;
	};
}
