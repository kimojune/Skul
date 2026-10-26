#pragma once
#include "yaGameObject.h"

namespace ya
{
	// 엔트 내려치기 판정. 주먹이 닿는 바닥 위에 잠깐(lifetime) 있다가 사라진다.
	// 플레이어(스컬)에 닿으면 PlayerStats::TakeDamage 로 한 번만 피해를 준다.
	// 대시 무적 중이면 TakeDamage 가 무시하므로, 판정이 남아 있는 동안 다시 시도한다.
	class EntSlamHitbox : public GameObject
	{
	public:
		// bottomCenter: 판정 사각형의 아래 가운데. sourcePos: 피해 방향 계산용 (엔트 몸 가운데)
		static EntSlamHitbox* Spawn(Vector2 bottomCenter, Vector2 size, int damage, Vector2 sourcePos, float lifetime);

		EntSlamHitbox();
		~EntSlamHitbox();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		virtual void OnCollisionEnter(class Collider* other) override;
		virtual void OnCollisionStay(class Collider* other) override;
		virtual void OnCollisionExit(class Collider* other) override;

	private:
		void tryHit(class Collider* other);

		Vector2 mSize;
		Vector2 mSourcePos;
		int mDamage;
		float mLifetime;
		float mTime;
		bool mbHit;
	};
}
