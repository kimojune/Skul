#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class Chapter1_Boss;

	// 보스 몸통(나무 줄기와 잎). Chapter1_Boss가 시키는 대로 기울거나(쓸기) 주저앉는다(그로기, 사망).
	// 충돌체는 그로기 때 몸 전체를 때릴 수 있게 하려고 있다. 그로기가 아니면 맞아도 피해가 없다.
	class Boss_Body : public GameObject
	{
	public:
		// 그림: 0 기본, 1 2페이즈(보라색), 2 2페이즈 전환 중 번쩍임
		enum class eLook
		{
			Normal,
			Phase2,
			Flash,
		};

		Boss_Body();
		~Boss_Body();

		virtual void Initialize()override;
		virtual void Update()override;
		virtual void Render(HDC hdc)override;
		virtual void Release()override;

		virtual void OnCollisionEnter(class Collider* other) override;
		virtual void OnCollisionStay(class Collider* other) override;
		virtual void OnCollisionExit(class Collider* other) override;

		void SetBoss(Chapter1_Boss* boss) { mBoss = boss; }
		// 처음 위치 기준으로 offset만큼 speed(px/초)로 옮긴다
		void SetOffset(Vector2 offset, float speed);
		void SnapOffset(Vector2 offset);
		bool IsArrived() { return mbArrived; }
		void SetLook(eLook look);

	private:
		Chapter1_Boss* mBoss;
		Animator* mAnimator;
		Vector2 mHomePos;
		Vector2 mOffset;
		Vector2 mTargetOffset;
		float mSpeed;
		bool mbArrived;
		eLook mLook;
	};
}
