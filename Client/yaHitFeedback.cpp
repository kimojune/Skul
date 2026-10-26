#include "yaHitFeedback.h"
#include "yaTime.h"
#include "yaCamera.h"
#include "yaAnimator.h"
#include "yaSound.h"
#include "yaResources.h"
#include "yaScenario.h"
#include "yaSceneManager.h"
#include "yaPlayeScene.h"
#include "yaSkul.h"

namespace ya
{
	float HitFeedback::mHitStopTime = 0.0f;
	float HitFeedback::mSlowMoTime = 0.0f;
	int HitFeedback::mHitStopCount = 0;
	int HitFeedback::mSlowMoCount = 0;
	bool HitFeedback::mbTimeFxEnabled = true;
	bool HitFeedback::mbEntKnockbackEnabled = true;
	int HitFeedback::mHitSoundIndex = 0;
	Sound* HitFeedback::mHitSounds[3] = {};

	void HitFeedback::Initialize()
	{
		mHitStopTime = 0.0f;
		mSlowMoTime = 0.0f;
		mHitStopCount = 0;
		mSlowMoCount = 0;
		Time::SetTimeScale(1.0);

		// 시나리오 대본이 시작할 때 이름을 확인하므로 미리 내보낸다
		Scenario::Publish("fx.timescale", 1.0);
		Scenario::Publish("fx.hitstops", 0.0);
		Scenario::Publish("fx.flashing", 0.0);
		Scenario::Publish("fx.slowmo", 0.0);
	}

	void HitFeedback::Update()
	{
		// 히트스톱이 슬로모션보다 먼저다. 히트스톱 중에는 슬로모션 시간도 흐르지 않는다.
		// 시간은 배율을 적용하지 않은 실제 시간으로 줄이므로 배율이 0이어도 반드시 끝난다
		double scale = 1.0;
		float dt = (float)Time::UnscaledDeltaTime();
		if (mHitStopTime > 0.0f)
		{
			if (mbTimeFxEnabled)
				scale = 0.0;
			mHitStopTime -= dt;
		}
		else if (mSlowMoTime > 0.0f)
		{
			if (mbTimeFxEnabled)
				scale = HitFx::SlowMoScale;
			mSlowMoTime -= dt;
		}
		if (mHitStopTime < 0.0f)
			mHitStopTime = 0.0f;
		if (mSlowMoTime < 0.0f)
			mSlowMoTime = 0.0f;

		Time::SetTimeScale(scale);

		Scenario::Publish("fx.timescale", scale);
		Scenario::Publish("fx.hitstops", mHitStopCount);
		Scenario::Publish("fx.flashing", Animator::GetFlashingCount());
		Scenario::Publish("fx.slowmo", mSlowMoCount);
	}

	bool HitFeedback::IsStrongHit(bool kill)
	{
		if (kill)
			return true;

		PlayeScene* scene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
		Skul* skul = (scene != nullptr) ? scene->GetSkul() : nullptr;
		return skul != nullptr && skul->IsSecondComboHit();
	}

	void HitFeedback::EnemyHit(bool kill)
	{
		bool strong = IsStrongHit(kill);

		// 타격 확인 소리: 세 개를 돌려가며
		if (mHitSounds[0] == nullptr)
		{
			mHitSounds[0] = Resources::Load<Sound>(L"Skul_Hit_Confirm1", L"..\\Resources\\Sound\\Skul\\Skul_Hit 1 (Unused).wav");
			mHitSounds[1] = Resources::Load<Sound>(L"Skul_Hit_Confirm2", L"..\\Resources\\Sound\\Skul\\Skul_Hit 2 (b copy) (Unused).wav");
			mHitSounds[2] = Resources::Load<Sound>(L"Skul_Hit_Confirm3", L"..\\Resources\\Sound\\Skul\\Skul_Hit 3 (b copy) (Unused).wav");
		}
		Sound* sound = mHitSounds[mHitSoundIndex];
		mHitSoundIndex = (mHitSoundIndex + 1) % 3;
		if (sound != nullptr)
			sound->Play(false);

		if (strong)
			Camera::Shake(HitFx::ShakeStrongPower, HitFx::ShakeStrongTime);
		else
			Camera::Shake(HitFx::ShakeNormalPower, HitFx::ShakeNormalTime);

		HitStop(strong ? HitFx::HitStopStrong : HitFx::HitStopNormal);
	}

	void HitFeedback::PlayerHurt()
	{
		HitStop(HitFx::HitStopPlayerHurt);
	}

	void HitFeedback::HitStop(float duration)
	{
		mHitStopCount++;
		if (duration > mHitStopTime)
			mHitStopTime = duration;
	}

	void HitFeedback::SlowMo()
	{
		mSlowMoCount++;
		if (HitFx::SlowMoTime > mSlowMoTime)
			mSlowMoTime = HitFx::SlowMoTime;
	}
}
