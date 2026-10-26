#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;

	// 보스 연출용 한 번 재생 이펙트. Chapter1_Boss가 미리 만들어 두고(Pause) 돌려 쓴다.
	// 위치는 그림의 아래쪽 가운데다 (바닥 높이를 주면 바닥에서 피어오른다).
	class BossEffect : public GameObject
	{
	public:
		enum class eType
		{
			AppearanceImpact,	// 등장·주먹 착지 흙먼지 (Boss/Effect/Appearance Impact)
			SlamSmoke,			// 양손 3연타 바닥 연기 (Boss/Effect/smoke, 32비트 원본을 마젠타 배경판으로 변환)
			DeathExplosion,		// 사망 폭발 연기 (Boss/Effect/Bullet/Smoke = smoke_origin의 마젠타 배경판)
			End,
		};

		BossEffect();
		~BossEffect();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		void Play(eType type, Vector2 bottomCenter);
		bool IsFree() { return GetState() != eState::Active; }

	private:
		void complete();

	private:
		Animator* mAnimator;
	};

	// 양손 3연타 예고: 바닥이 붉게 빛난다.
	// Effect/Range(세로 경고 기둥)의 바닥 쪽 밝은 줄을 가로로 늘려 그리고, 그 위에 붉은 빛을 반투명 띠로 겹친다.
	class BossTelegraph : public GameObject
	{
	public:
		static constexpr float Width = 650.0f;	// 예고 구역 너비 (두 주먹이 덮는 범위)

		BossTelegraph();
		~BossTelegraph();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		// centerX를 가운데로 바닥에 예고를 띄운다
		void Show(float centerX);
		void Hide();

	private:
		Animator* mAnimator;
		float mCenterX;
		float mTime;
	};
}
