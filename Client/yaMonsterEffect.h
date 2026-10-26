#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;

	// 몬스터가 남기는 효과 그림. 충돌체 없이 Effect 레이어에 그려지기만 한다.
	// 한 번 재생하는 효과는 애니메이션이 끝나면 스스로 사라진다.
	// EntWarning 은 반복 재생이라 만든 쪽(엔트)이 SetState(Death)로 지운다.
	class MonsterEffect : public GameObject
	{
	public:
		enum class eType
		{
			EntSlamSmoke,	// 주먹이 바닥에 닿을 때 먼지 (Land_Smoke). pos = 바닥 가운데
			EntWarning,		// 내려치기 예고: 바닥의 붉은 빛 (Range). pos = 바닥 가운데
			EntDeathBurst,	// 쓰러질 때 폭발 (Range_Attack). pos = 몸 가운데
		};

		// object::Instantiate 와 같지만 종류를 생성자로 넘긴다
		static MonsterEffect* Spawn(eType type, Vector2 pos);

		MonsterEffect(eType type);
		~MonsterEffect();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

	private:
		Animator* mAnimator;
		eType mType;
		float mTime;
	};
}
