#pragma once
#include "YamYamEngine.h"

namespace ya
{
	class Sound;

	// 타격감 튜닝값. 느낌을 바꾸고 싶으면 여기만 고치면 된다.
	namespace HitFx
	{
		// 1. 히트스톱 (초, 시간 배율과 무관한 실제 시간). 시간 배율 0으로 멈춘다
		// (플레이테스트: 0.06/0.10/0.08은 답답했다 → 절반으로)
		constexpr float HitStopNormal = 0.03f;		// 일반 타격
		constexpr float HitStopStrong = 0.05f;		// 2타째와 처치
		constexpr float HitStopPlayerHurt = 0.04f;	// 플레이어가 맞았을 때

		// 2. 흰색 번쩍임 (초)
		constexpr float FlashTime = 0.08f;

		// 4. 카메라 흔들림 (px, 초)
		constexpr float ShakeNormalPower = 2.5f;
		constexpr float ShakeNormalTime = 0.06f;
		constexpr float ShakeStrongPower = 5.0f;
		constexpr float ShakeStrongTime = 0.12f;

		// 5. 엔트 넉백(밀려나는 거리 px)과 경직(초).
		//    밀리는 건 처음 EntKnockbackTime 동안만 빠르게 하고 나머지 경직은 제자리.
		//    (플레이테스트: 60/110px를 경직 내내 밀면 미끄러져 보이고, 2타째가 멀어 다음 공격이 안 닿았다)
		//    공격 돌진(35px)보다 작게 둬서, 붙어서 연타하면 계속 닿는다
		constexpr float EntKnockbackNormal = 20.0f;
		constexpr float EntKnockbackStrong = 30.0f;
		constexpr float EntKnockbackTime = 0.1f;
		constexpr float EntStunNormal = 0.4f;
		constexpr float EntStunStrong = 0.55f;

		// 6. 공격 돌진 (px). 공격 시작 후 LungeTime 동안만 빠르게 나가고 멈춘다
		//    (플레이테스트: 애니메이션 내내 나가면 미끄러져 보였다. 거리는 1·2타 모두 35px로 고정)
		constexpr float LungeHit1 = 35.0f;
		constexpr float LungeHit2 = 35.0f;
		constexpr float LungeTime = 0.08f;

		// 7. 슬로모션: 마지막 엔트 처치, 보스 페이즈 전환, 보스 사망 (실제 시간 초)
		constexpr float SlowMoScale = 0.3f;
		constexpr float SlowMoTime = 0.4f;
	}

	// 히트스톱과 슬로모션의 시간 배율을 한곳에서 정하고, 타격 확인 소리·흔들림을 낸다.
	// Application::Update 가 매 로직 단계 처음에 Update 를 부른다.
	class HitFeedback
	{
	public:
		// 시나리오 변수 초깃값을 내보내고 상태를 비운다
		static void Initialize();
		static void Update();

		// 플레이어 공격이 적(엔트, 보스 부위)에 들어갔다. kill 이면 처치(또는 보스 페이즈 전환·사망) 타격
		// 소리, 카메라 흔들림, 히트스톱을 낸다. 2타째인지는 플레이어 상태에서 읽는다
		static void EnemyHit(bool kill);
		// 이번 타격이 센 타격(2타째 또는 처치)인가
		static bool IsStrongHit(bool kill);
		// 플레이어가 맞았다 (히트스톱)
		static void PlayerHurt();
		// 남은 시간과 비교해 더 긴 쪽만 남긴다 (더하지 않는다)
		static void HitStop(float duration);
		static void SlowMo();

		// 시나리오 `timefx off`: 시간 배율만 끈다 (소리, 흔들림, 번쩍임은 그대로)
		static void SetTimeFxEnabled(bool enabled) { mbTimeFxEnabled = enabled; }
		static bool IsTimeFxEnabled() { return mbTimeFxEnabled; }
		// 시나리오 `fxknock off`: 엔트 넉백·경직을 예전 값(약 18px, 0.3초)으로 되돌리고 공격 돌진을 끈다
		static void SetEntKnockbackEnabled(bool enabled) { mbEntKnockbackEnabled = enabled; }
		static bool IsEntKnockbackEnabled() { return mbEntKnockbackEnabled; }

	private:
		static float mHitStopTime;
		static float mSlowMoTime;
		static int mHitStopCount;
		static int mSlowMoCount;
		static bool mbTimeFxEnabled;
		static bool mbEntKnockbackEnabled;
		static int mHitSoundIndex;
		static Sound* mHitSounds[3];
	};
}
