#pragma once
#include "YamYamEngine.h"

namespace ya
{
	// 플레이어 공용 상태. Basic/Nohead 두 스컬이 번갈아 조작되므로 HP는 여기 한 곳에 둔다.
	// 적·보스는 TakeDamage로 피해를 주고, 플레이어 쪽 반응(넉백, 상태 전환)은 OnDamaged 콜백으로 연결한다.
	// 원작 리틀 본 기준: 최대 HP 100, 피격 후 무적 없음, 대시 중 무적 (docs/ai-rework/04-original-reference.md)
	class PlayerStats
	{
	public:
		static void Reset();

		static int GetHP() { return mHP; }
		static int GetMaxHP() { return mMaxHP; }
		static bool IsDead() { return mHP <= 0; }

		// 무적이거나 이미 죽었으면 무시한다. 실제로 피해를 입었으면 true
		static bool TakeDamage(int damage, Vector2 sourcePos);
		static void Heal(int amount);

		static void SetInvincible(bool invincible) { mbInvincible = invincible; }
		static bool IsInvincible() { return mbInvincible; }

		static void SetOnDamaged(const std::function<void(int damage, Vector2 sourcePos)>& callback) { mOnDamaged = callback; }

	private:
		static int mHP;
		static int mMaxHP;
		static bool mbInvincible;
		static std::function<void(int, Vector2)> mOnDamaged;
	};
}
