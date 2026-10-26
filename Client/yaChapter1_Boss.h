#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Boss_Body;
	class Boss_Head;
	class Boss_Hand;
	class Boss_RightHand;
	class Boss_Chin;
	class BossBullet;
	class BossEffect;
	class BossTelegraph;
	class BossHpBar;
	class Platform;
	class Sound;

	// 보스 부품 이동 도우미: current를 target 쪽으로 최대 maxStep(px)만큼 옮긴다. 도착하면 true
	inline bool BossMoveToward(Vector2& current, Vector2 target, float maxStep)
	{
		Vector2 toTarget = target - current;
		float distance = toTarget.Length();
		if (maxStep < 0.0f || distance <= maxStep)
		{
			current = target;
			return true;
		}
		current += toTarget * (maxStep / distance);
		return false;
	}

	// 1장 보스 장로 엔트 "위그드라실".
	// 몸통·머리·턱·두 손은 따로 그려지는 부품(Boss_*)이고, 이 오브젝트는 그리지 않고 패턴만 지휘한다.
	// 부품은 "어디로 얼마나 빨리 가라, 어떤 포즈를 해라, 지금 피해를 줘라"만 받는다.
	// 원작 동작 기준: docs/ai-rework/04-original-reference.md
	class Chapter1_Boss : public GameObject
	{
	public:
		// boss.state 로 내보내는 값. 시나리오는 숫자로 비교하므로 순서를 바꾸면 Scenarios/g4_*.txt도 고친다
		enum class eBossState
		{
			Intro,			// 0 등장 연출 (땅을 움켜쥠)
			Idle,			// 1 패턴 사이 대기
			Punch,			// 2 주먹 내려치기
			Sweep,			// 3 바닥 쓸기
			EnergyBall,		// 4 에너지볼 3회 (1페이즈) -> 그로기
			Groggy,			// 5 그로기. 몸 전체를 때릴 수 있다
			PhaseChange,	// 6 2페이즈 전환 연출
			EnergyRain,		// 7 에너지볼 비 (2페이즈) -> 그로기
			TripleSlam,		// 8 양손 3연타 (2페이즈)
			Dead,			// 9 사망
			End,
		};

		// 플레이어 공격에 맞은 부품
		enum class ePart
		{
			Head,
			Body,
			Hand,
		};

		// ---- HP ----
		// 원작은 1페이즈 1080 + 2페이즈 1080 = 2160이고, 1장 초반 리틀 본의 기본 공격은 한 대에 수십 정도다.
		// 2160 ÷ 25~30 ≈ 70~85대. 클론은 지금 플레이어 공격 한 대가 1이므로 페이즈마다 40대, 모두 80대로 잡았다.
		// 그로기 한 번(약 4초)에 10대 안팎을 넣을 수 있어서, 대략 2~3분 걸리는 싸움이 된다.
		static constexpr int Phase1HP = 40;
		static constexpr int Phase2HP = 40;
		static constexpr int MaxHP = Phase1HP + Phase2HP;

		// ---- 플레이어가 받는 피해 (플레이어 최대 HP 100) ----
		// 원작 1장: 큰 엔트 10 이상, 중간보스 단발 10~20. 보스는 그 범위 위쪽으로 잡았다.
		static constexpr int FistDamage = 15;			// 주먹 내려치기
		static constexpr int SweepDamage = 15;			// 바닥 쓸기
		static constexpr int SmallBallDamage = 10;		// 8방향 작은 구체
		static constexpr int BigBallDamage = 15;		// 조준 큰 구체
		static constexpr int RainBallDamage = 12;		// 2페이즈 에너지볼 비
		static constexpr int TripleSlamDamage = 20;		// 2페이즈 양손 3연타 (한 번마다)

		// ---- 2페이즈 보라색 톤 (R 작업 후 연결) ----
		// 보라색 그림이 있는 부품(_2 손, Body_2, Head_3, Chin_2)은 그림을 바꿔 표현한다.
		// 오른손 대기·바닥 짚기는 보라색 그림이 없어서, 렌더러가 색 곱하기(tint)를 지원하면
		// IsPhase2Tinted()가 true인 동안 이 색을 곱해 주면 된다. 지금 GDI 렌더러에서는 쓰지 않는다.
		static constexpr COLORREF Phase2Tint = RGB(190, 120, 255);

		// 페이즈마다 다른 패턴 수치. 2페이즈가 더 빠르다 (값은 yaChapter1_Boss.cpp)
		struct Timing
		{
			float idle;				// 패턴 사이 대기 (초)
			float punchRaise;		// 두 주먹을 들고 떠는 시간 (예고)
			float punchSlamSpeed;	// 내려치는 속도 (px/초)
			float punchStay;		// 내려친 주먹이 바닥에 머무는 시간
			int punchCount;			// 내려치기 횟수 (2페이즈는 2+2)
			float sweepReady;		// 손을 화면 밖으로 벌리고 기울어진 뒤 쓸기까지 (예고)
			float sweepSpeed;		// 쓸기 속도 (px/초)
			int sweepCount;			// 쓸기 횟수 (2페이즈는 좌우 2+2)
			float groggy;			// 그로기 시간
		};

		Chapter1_Boss();
		~Chapter1_Boss();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		// 머리는 SkulAttack이 Boss_Head::Hit()으로 알려 준다 (판정은 SkulAttack 쪽에서 끝난 상태).
		// 피해가 들어갔으면 true (등장·전환·사망 연출 중에는 false)
		bool OnHeadHit();
		// 몸통·손이 플레이어 공격 판정(SkulAttack)과 겹쳤을 때 부른다. 그로기일 때만 피해를 받는다
		// 피해가 들어갔으면 true (호출한 부위가 번쩍인다)
		bool OnPartAttacked(ePart part);

		eBossState GetBossState() { return mState; }
		int GetHP() { return mHP; }
		int GetPhase() { return mPhase; }
		bool IsGroggy() { return mbGroggy; }
		bool IsDead() { return mState == eBossState::Dead; }
		bool IsBarVisible() { return mbBarVisible; }
		bool IsPhase2Tinted() { return mPhase == 2 && mState != eBossState::Dead; }

	private:
		void enterState(eBossState state);
		void setStep(int step);
		const Timing& timing();
		void publish();

		void intro();
		void idle();
		void punch();
		void sweep();
		void energyBall();
		void groggy();
		void phaseChange();
		void energyRain();
		void tripleSlam();
		void dead();

		bool applyDamage(int amount);
		void debugSkipPhase();
		eBossState pickPattern();

		// 부품 조작 도우미. 손 위치는 손 그림의 아래쪽 가운데
		Boss_Hand* hand(bool right);
		Vector2 hoverHome(bool right);		// 대기 자리
		Vector2 raisedPos(bool right);		// 주먹을 들어 올린 자리
		Vector2 restPos(bool right);		// 손바닥으로 바닥을 짚는 자리
		Vector2 sweepOutside(bool right);	// 쓸기 시작·끝 (맵 밖)
		void handsHome(float speed);
		void handsRest(float speed);
		void setPartsOffset(Vector2 offset, float speed);
		void setPartsPhase2(bool phase2);
		void stopAllBullets();
		BossBullet* acquireBullet();
		BossEffect* playEffect(int type, Vector2 bottomCenter);
		Vector2 playerPos();

	private:
		Boss_Body* mBody;
		Boss_Head* mHead;
		Boss_Chin* mChin;
		Boss_Hand* mLeftHand;
		Boss_RightHand* mRightHand;

		// 미리 만들어 두고 돌려 쓰는 개수. 에너지볼 3회 중 동시에 살아 있는 탄은 최대 19개쯤(9 + 9 + 충전 1),
		// 비는 13개. 이펙트는 사망 폭발 때 5개쯤 겹친다
		static constexpr int BulletCount = 24;
		static constexpr int EffectCount = 8;
		BossBullet* mBullets[BulletCount];
		BossEffect* mEffects[EffectCount];
		BossTelegraph* mTelegraph;
		BossHpBar* mHpBar;
		Platform* mPlatform[2];

		eBossState mState;
		eBossState mLastPattern;
		float mStateTime;		// 상태에 들어온 뒤 지난 시간
		float mStepTime;		// 현재 단계에 들어온 뒤 지난 시간
		int mStep;				// 패턴 안의 단계
		bool mbStepEnter;		// 이번 프레임이 단계의 첫 프레임인지
		bool mbStepChanged;		// 이번 프레임에 단계가 바뀌었는지
		int mCount;				// 패턴 안에서 반복한 횟수 (내려치기, 쓸기, 에너지볼 발사 등)
		bool mSweepRight;		// 이번 쓸기가 오른쪽에서 시작하는지
		bool mSlamRight;		// 이번에 내려치는 주먹이 오른손인지
		float mSlamCenterX;		// 양손 3연타 목표 위치
		float mCenterX;			// 몸 가운데(이마 마석) x
		BossBullet* mBigBall;	// 충전 중인 조준 큰 구체

		int mHP;
		int mPhase;
		int mVolleys;			// 이번 에너지볼 패턴에서 쏜 횟수 (boss.volleys)
		bool mbGroggy;
		bool mbBarVisible;

		Sound* mAppearance;
		Sound* mRoar;
		Sound* mRoarShort;
		Sound* mFistSlam;
		Sound* mFistSlamRecovery;
		Sound* mSweepingReady;
		Sound* mSweeping;
		Sound* mEnergyReady;
		Sound* mEnergyFire;
		Sound* mGroggyImpact;
		Sound* mGroggyRecovery;
		Sound* mDown;
		Sound* mUp;
		Sound* mBothFistRoar;
		Sound* mDeadImpact;
		Sound* mDeadRoar;
	};
}
