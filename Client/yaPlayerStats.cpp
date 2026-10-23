#include "yaPlayerStats.h"

namespace ya
{
	int PlayerStats::mHP = 100;
	int PlayerStats::mMaxHP = 100;
	bool PlayerStats::mbInvincible = false;
	std::function<void(int, Vector2)> PlayerStats::mOnDamaged = nullptr;

	void PlayerStats::Reset()
	{
		mHP = mMaxHP;
		mbInvincible = false;
	}

	bool PlayerStats::TakeDamage(int damage, Vector2 sourcePos)
	{
		if (damage <= 0 || mbInvincible || IsDead())
			return false;

		mHP -= damage;
		if (mHP < 0)
			mHP = 0;

		if (mOnDamaged)
			mOnDamaged(damage, sourcePos);
		return true;
	}

	void PlayerStats::Heal(int amount)
	{
		if (amount <= 0 || IsDead())
			return;

		mHP += amount;
		if (mHP > mMaxHP)
			mHP = mMaxHP;
	}
}
