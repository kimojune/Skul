#pragma once
#include "YamYamEngine.h"

namespace ya
{
	// 플레이어 공용 상태. Basic/Nohead 두 스컬이 번갈아 조작되므로 HP는 여기 한 곳에 둔다.
	// 적·보스는 TakeDamage로 피해를 주고, 플레이어 쪽 반응(넉백, 상태 전환)은 OnDamaged 콜백으로 연결한다.
	// 원작 리틀 본 기준: 최대 HP 100, 피격 후 무적 없음, 대시 중 무적 (docs/ai-rework/04-original-reference.md)
	//
	// 점프 횟수, 대시 충전, 스킬 쿨다운, 머리가 나가 있는지도 여기 둔다.
	// 머리 있는 몸(Basic)과 머리 없는 몸(Nohead)은 같은 캐릭터라 교대해도 이 값이 이어져야 하고, HUD도 이 값을 읽는다.
	class PlayerStats
	{
	public:
		// 원작 수치. "불확실"은 원작 수치를 찾지 못해 이 클론에서 정한 값이다.
		static constexpr int MaxJumps = 2;					// 2단 점프 (확인됨)
		static constexpr int MaxDashCharges = 2;			// 대시 2회 충전 (확인됨)
		static constexpr float DashRechargeTime = 1.0f;		// 대시 1회분이 다시 차는 시간. 한 번에 1회씩 찬다 (불확실)
		static constexpr float SkillACooldownTime = 6.0f;	// A 두개골 투척 (확인됨)
		static constexpr float SkillSCooldownTime = 3.0f;	// S 순간이동 (확인됨)

		// HP를 채우고 무적·점프·대시·쿨다운·머리 상태를 처음으로 돌린다
		static void Reset();
		// 대시 충전과 스킬 쿨다운을 진행한다. 플레이 씬(PlayeScene::Update)이 매 프레임 부른다
		static void Update(float deltaTime);
		// 시나리오 상태 변수로 내보낸다 (player.dashCharges 등. Scenarios/README.md 참고)
		static void Publish();

		static int GetHP() { return mHP; }
		static int GetMaxHP() { return mMaxHP; }
		static bool IsDead() { return mHP <= 0; }

		// 무적이거나 이미 죽었으면 무시한다. 실제로 피해를 입었으면 true
		static bool TakeDamage(int damage, Vector2 sourcePos);
		static void Heal(int amount);

		static void SetInvincible(bool invincible) { mbInvincible = invincible; }
		static bool IsInvincible() { return mbInvincible; }

		static void SetOnDamaged(const std::function<void(int damage, Vector2 sourcePos)>& callback) { mOnDamaged = callback; }

		// 점프: 땅에서 뛴 것을 포함해 착지 전까지 MaxJumps번. 남았으면 1회 쓰고 true
		static bool UseJump();
		static int GetJumpCount() { return mJumpCount; }

		// 대시: 남은 충전이 있으면 1회 쓰고 true
		static bool UseDashCharge();
		static int GetDashCharges() { return mDashCharges; }

		// 착지 중에 부른다. 점프 횟수와 공중 투척 1회 제한을 되돌린다
		static void OnGrounded();

		// A 두개골 투척: 머리가 붙어 있고, 쿨다운이 끝났고, 공중이면 이번 공중에서 아직 던지지 않았을 때
		static bool CanThrowHead(bool grounded);
		static void OnHeadThrown(bool grounded);
		// 머리를 되찾았다. 줍거나 순간이동으로 회수하면 A 쿨다운이 초기화된다 (resetCooldown)
		static void OnHeadRetrieved(bool resetCooldown);
		static bool IsHeadOut() { return mbHeadOut; }
		static float GetSkillACooldown() { return mSkillACooldown; }

		// S 순간이동: 머리가 나가 있고 쿨다운이 끝났을 때
		static bool CanTeleport();
		static void OnTeleported();
		static float GetSkillSCooldown() { return mSkillSCooldown; }

	private:
		static int mHP;
		static int mMaxHP;
		static bool mbInvincible;
		static std::function<void(int, Vector2)> mOnDamaged;

		static int mJumpCount;
		static int mDashCharges;
		static float mDashRecharge;		// 다음 1회가 차기까지 남은 시간
		static bool mbAirThrowUsed;
		static bool mbHeadOut;
		static float mSkillACooldown;	// 남은 시간 (초)
		static float mSkillSCooldown;
	};
}
