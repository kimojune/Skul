#pragma once
#include "yaGameObject.h"
#include "yaSkulHead.h"

namespace ya
{
	class Animator;
	class Rigidbody;
	class SkulAttack;
	class Sound;
	class Skul : public GameObject
	{
	public:
		enum class eSkulType
		{
			Basic,
			Nohead,
			End,
		};

		enum class eSkulState
		{
			Idle,
			Move,
			Dash,
			Jump,
			Fall,
			Attack,
			Attack2,
			JumpAttack,
			SkillA,
			SkillS,
			ChangeSkill,
			Fall_Dead,

		};

		// 피격 반응. 원작에 기록이 없어(불확실) 이 클론에서 정한 값이다.
		// 경직 동안은 입력을 받지 않고, 맞은 반대쪽으로 조금 밀린다 (300px/s x 0.15초 = 약 45px)
		static constexpr float HitStunTime = 0.15f;
		static constexpr float KnockbackSpeed = 300.0f;
		// 사망: 쓰러지는 애니메이션을 보여 준 뒤 이 시간에 타이틀로 돌아간다. 마지막 FadeTime 동안 화면이 어두워진다
		static constexpr float DeadTime = 2.0f;
		static constexpr float DeadFadeTime = 0.5f;

		Skul();
		~Skul();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;


		virtual void OnCollisionEnter(class Collider* other) override;
		virtual void OnCollisionStay(class Collider* other) override;
		virtual void OnCollisionExit(class Collider* other) override;


		Skul* SwitchSkul(eSkulType type);
		void SetDirect(eDirection direct);
		bool CheckAnimation(const std::wstring& name);
		eDirection GetDirect() { return mDirect;}
		bool GetIsAttack() { return IsAttack; }
		void SetIsAttack(bool mtrue) { IsAttack = mtrue; }


		void SetSkulState(eSkulState state) { mState = state; }
		eSkulState GetSkulState() {return mState; }

		// PlayerStats의 OnDamaged 콜백이 조작 중인 스컬에게 전달한다. HP가 0이면 Die
		void OnDamaged(int damage, Vector2 sourcePos);
		// 쓰러지는 애니메이션을 재생하고 입력을 막는다. 머리 없는 몸은 머리를 되찾은 뒤 쓰러진다 (Nohead::Die)
		virtual void Die();
		// 사망 연출이 끝났는가 (플레이 씬이 보고 타이틀로 돌아간다)
		bool IsDeathFinished() { return mState == eSkulState::Fall_Dead && mDeadTime >= DeadTime; }
		// 사망 뒤 다시 시작할 때 Stage1 시작 상태로 돌린다.
		// Phase 3에서 씬 리셋·결과 화면으로 바꿀 때까지 쓰는 최소한의 초기화다 (스테이지 상태는 그대로)
		void ResetForNewRun();

		void StartDash();
		void CompleteDash();
		void EndDash();

		void StartAttack();
		void CompleteAttack();
		void EndAttack();

		void StartJumpAttack();
		void CompleteJumpAttack();
		void EndJumpAttack();

		void StartJump();
		void CompleteJump();
		void EndJump();

		void StartFall();
		void CompleteFall();
		void EndFall();

		virtual void StartSkillA();
		virtual void CompleteSkillA();
		virtual void EndSkillA();

		virtual void StartSkillS();
		virtual void CompleteSkillS();
		virtual void EndSkillS();

		virtual void StartChangeSkill();
		virtual void CompleteChangeSkill();
		virtual void EndChangeSkill();




	protected:
		void	Idle();
		void	Move();
		void	Dash();
		void	Jump();
		void	Fall();
		void	Attack();
		void	FallDead();

		virtual void	SkillA();
		virtual void	SkillS();
		virtual void	ChangeSkill();
		void	Jumpattack();

		// A·S 키를 받을 수 있는가. Basic은 A(던지기)만, Nohead는 S(순간이동)만 쓴다
		virtual bool CanUseSkillA() { return false; }
		virtual bool CanUseSkillS() { return false; }

		// 대시 충전·점프 횟수가 남았으면 상태를 바꾸고 true
		bool startDash();
		bool startJump();
		void playDirectional(const wchar_t* left, const wchar_t* right, bool loop);

		eDirection mDirect;
		eSkulState mState;

	private:
		void updateHitStun();
		void updateDashInvincible();

		Animator* mAnimator;
		Rigidbody* mRigidbody;
		SkulAttack* mAttack;

		bool SecondAttack;
		bool SecondDash;
		bool SecondJump;
		bool IsAttack;
		bool mbDashInvincible;	// 대시로 켠 무적 (대시가 끝나면 끈다)

		float mHitStunTime;		// 남은 경직 시간
		float mKnockback;		// 경직 동안 x 이동 속도 (부호가 방향)
		float mDeadTime;		// 쓰러진 뒤 흐른 시간

		//Skul* mSecondSkul;

		UINT AttackCount;

		Sound* Default_Dash;
		Sound* Skul_Jump_Atk;
		Sound* Skul_Atk1;
		Sound* Skul_Atk2;
		Sound* Skul_SkullThrowing;
		Sound* Skul_Hit_1;
		Sound* Default_Jump;
		Sound* Default_Jump_Air;
		Sound* Default_Dead;

		Sound* mDash;


	};

}
