#pragma once

#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class Chapter1_Boss;

	// 보스 머리. 그로기가 아닐 때 피해를 받는 유일한 부위다.
	// 플레이어 공격 판정(SkulAttack, G2 담당)이 머리와 겹치면 Hit()을 부르고, 피해 계산은 Chapter1_Boss가 한다.
	class Boss_Head : public GameObject
	{
	public:
		enum class eLook
		{
			Normal,	// 붉은 눈, 이마에 마석 (Head_1)
			Phase2,	// 보라색 (Head_3)
			Sane,	// 마석이 빠지고 제정신을 찾은 초록 눈 (Head_2, 사망 후)
		};

		Boss_Head();
		~Boss_Head();

		virtual void Initialize()override;
		virtual void Update()override;
		virtual void Render(HDC hdc)override;
		virtual void Release()override;

		void SetBoss(Chapter1_Boss* boss) { mBoss = boss; }
		// 처음 위치 기준으로 offset만큼 speed(px/초)로 옮긴다
		void SetOffset(Vector2 offset, float speed);
		void SnapOffset(Vector2 offset);
		bool IsArrived() { return mbArrived; }
		void SetLook(eLook look);
		// 숨 쉬듯 위아래로 흔들림 (쓰러져 있을 때는 끈다)
		void SetBob(bool bob) { mbBob = bob; }

		// 이마 마석 위치. 에너지볼이 여기서 모인다
		Vector2 GetGemPos();

		// SkulAttack이 부른다
		void Hit();

	private:
		Chapter1_Boss* mBoss;
		Animator* mAnimator;
		Vector2 mHomePos;
		Vector2 mOffset;
		Vector2 mTargetOffset;
		float mSpeed;
		bool mbArrived;
		bool mbBob;
		float mTime;
		float mHitShake;	// 맞았을 때 잠깐 흔들리는 남은 시간
		eLook mLook;
	};
}
