#pragma once
#include "yaGameObject.h"
#include "yaImage.h";

namespace ya
{
	class Skul;
	class Animator;
	class Sound;
	class BossBullet : public GameObject
	{
	public:
		BossBullet();
		~BossBullet();

		enum class eBulletState
		{
			Impact,
			Loop,
			Outro,
		};
		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		void SetPrevPos();
		void SetmPlay(bool play) { mPlayed = play; }

		

		virtual void OnCollisionEnter(class Collider* other);
		virtual void OnCollisionStay(class Collider* other);
		virtual void OnCollisionExit(class Collider* other);

		virtual void SetDirect(eDirection direct) { mDirect = direct; }

	protected:
		void Impact();
		void Loop();
		void Outro();

	private:
		Animator* mAnimator;
		float mTime;
		
		eBulletState mBulletState;

		Sound* ElderEnt_EnergyBomb_Ready;
		Sound* ElderEnt_EnergyBomb_Fire;
		Image* mLeftImage;
		Image* mRightImage;
		Vector2 mPrevPos;
		eDirection mDirect;
		Vector2 direction;
		bool mPlayed;
		bool mSound;
	};
}
