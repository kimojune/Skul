#include "yaPlayerStats.h"
#include "yaScenario.h"

namespace ya
{
	int PlayerStats::mHP = 100;
	int PlayerStats::mMaxHP = 100;
	bool PlayerStats::mbInvincible = false;
	std::function<void(int, Vector2)> PlayerStats::mOnDamaged = nullptr;

	int PlayerStats::mJumpCount = 0;
	int PlayerStats::mDashCharges = PlayerStats::MaxDashCharges;
	float PlayerStats::mDashRecharge = 0.0f;
	bool PlayerStats::mbAirThrowUsed = false;
	bool PlayerStats::mbHeadOut = false;
	float PlayerStats::mSkillACooldown = 0.0f;
	float PlayerStats::mSkillSCooldown = 0.0f;

	void PlayerStats::Reset()
	{
		mHP = mMaxHP;
		mbInvincible = false;

		mJumpCount = 0;
		mDashCharges = MaxDashCharges;
		mDashRecharge = 0.0f;
		mbAirThrowUsed = false;
		mbHeadOut = false;
		mSkillACooldown = 0.0f;
		mSkillSCooldown = 0.0f;

		Publish();
	}

	void PlayerStats::Update(float deltaTime)
	{
		// 대시는 한 번에 1회씩 찬다. 다 차 있으면 타이머가 돌지 않는다
		if (mDashCharges < MaxDashCharges)
		{
			mDashRecharge -= deltaTime;
			if (mDashRecharge <= 0.0f)
			{
				mDashCharges++;
				mDashRecharge = (mDashCharges < MaxDashCharges) ? DashRechargeTime : 0.0f;
			}
		}

		mSkillACooldown -= deltaTime;
		if (mSkillACooldown < 0.0f)
			mSkillACooldown = 0.0f;

		mSkillSCooldown -= deltaTime;
		if (mSkillSCooldown < 0.0f)
			mSkillSCooldown = 0.0f;

		Publish();
	}

	void PlayerStats::Publish()
	{
		Scenario::Publish("player.jumpCount", mJumpCount);
		Scenario::Publish("player.dashCharges", mDashCharges);
		Scenario::Publish("player.headOut", mbHeadOut ? 1.0 : 0.0);
		Scenario::Publish("player.skillACooldown", mSkillACooldown);
		Scenario::Publish("player.skillSCooldown", mSkillSCooldown);
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

	bool PlayerStats::UseJump()
	{
		if (mJumpCount >= MaxJumps)
			return false;

		mJumpCount++;
		return true;
	}

	bool PlayerStats::UseDashCharge()
	{
		if (mDashCharges <= 0)
			return false;

		// 다 차 있던 상태에서 쓰면 그때부터 충전 타이머가 돈다 (이미 돌고 있으면 그대로)
		if (mDashCharges == MaxDashCharges)
			mDashRecharge = DashRechargeTime;
		mDashCharges--;
		return true;
	}

	void PlayerStats::OnGrounded()
	{
		mJumpCount = 0;
		mbAirThrowUsed = false;
	}

	bool PlayerStats::CanThrowHead(bool grounded)
	{
		if (mbHeadOut || mSkillACooldown > 0.0f)
			return false;

		// 원작: 공중에서는 1회만 던질 수 있다 (공중에서 머리를 주워 쿨다운이 초기화돼도 착지 전에는 다시 못 던진다)
		if (!grounded && mbAirThrowUsed)
			return false;

		return true;
	}

	void PlayerStats::OnHeadThrown(bool grounded)
	{
		mbHeadOut = true;
		mSkillACooldown = SkillACooldownTime;
		if (!grounded)
			mbAirThrowUsed = true;
	}

	void PlayerStats::OnHeadRetrieved(bool resetCooldown)
	{
		mbHeadOut = false;
		if (resetCooldown)
			mSkillACooldown = 0.0f;
	}

	bool PlayerStats::CanTeleport()
	{
		return mbHeadOut && mSkillSCooldown <= 0.0f;
	}

	void PlayerStats::OnTeleported()
	{
		mSkillSCooldown = SkillSCooldownTime;
	}
}
