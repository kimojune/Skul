#pragma once
#include "yaGameObject.h"


namespace ya
{
	class Animator;
	class Rigidbody;
	class Sound;
	class Skul;
	class MonsterEffect;

	// 1장 엔트. 플레이어를 발견하면 천천히 다가가서, 주먹을 들어 올린 채 떨다가(예고) 내려친다.
	// 수치와 타이밍은 yaMonster.cpp 의 EntTuning 에 모아 두었다.
	class Monster : public GameObject
	{
	public:
		Monster();
		~Monster();

		enum class eMonsterState
		{
			Idle,
			Move,
			Attack,
			LangeAttack,
			Hit,
			Dead,

		};

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		eMonsterState GetMonsterState() { return mState; }
		// Hit 을 넘기면 피해 1을 그 자리에서 적용한다 (SkulAttack, SkulHead 가 부른다).
		void SetMonsterState(eMonsterState state);
		int GetHP() { return mHP; }

		//virtual void OnCollisionEnter(class Collider* other) override;
		//virtual void OnCollisionStay(class Collider* other) override;
		//virtual void OnCollisionExit(class Collider* other) override;


	protected:

		virtual void Idle();
		virtual void Move();
		virtual void Attack();
		virtual void LangeAttack();
		virtual void Hit();
		virtual void Dead();

		void CompleteIdle();
		void CompleteHit();
		void EndIdle();
		void EndHit();

	private:
		enum class eAttackPhase
		{
			Windup,		// 주먹을 들고 떠는 예고
			Recover,	// 내려친 뒤 주먹을 바닥에 댄 채 멈춤
		};

		void think();
		void wander();
		void changeMove(eMonsterState state, eDirection direction);
		void playAnimation(const std::wstring& action, bool loop);
		void takeHit(int damage);
		// 살아 있는(죽는 중이 아닌) 엔트가 이 엔트 말고 없는가
		bool isLastEnt();
		void startWindup();
		void slam();
		void cancelAttack();
		void killWarning();
		bool clampToMap(Vector2& pos);
		void createPoseAnimation(const std::wstring& name, const std::wstring& path);
		void publishStats();

		Skul* findPlayer();
		float getBodyCenterX();
		Vector2 getFistPoint();

		Animator* mAnimator;
		eMonsterState mState;
		Rigidbody* mRigidbody;
		eDirection mDirection;
		eAttackPhase mAttackPhase;
		MonsterEffect* mWarning;	// 예고 중에만 있다. 이 엔트가 지운다

		Sound* mWindupSound;
		Sound* mSlamSound;
		Sound* mDeadSound;

		int mHP;
		float mTime;		// 배회: 다음 행동을 고를 때까지 쌓는 시간
		float mStunTime;		// 이번 피격의 경직 시간
		float mKnockbackSpeed;	// 이번 피격의 처음 넉백 속도 (경직이 끝날 때까지 0으로 줄어든다)
		float mDelay;		// 지금 상태(피격, 공격 단계, 죽는 중)에 들어온 뒤 지난 시간
		float mCooldown;	// 다음 내려치기까지 남은 시간
		float mHomeX;		// 배치된 x. 배회는 이 근처에서만 한다
		float mAnchorX;		// 예고 중 떨기 전의 원래 x
		bool mbAggro;		// 플레이어를 쫓는 중
	};
}
