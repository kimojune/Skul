#pragma once
#include "yaGameObject.h"
#include "yaImage.h"

namespace ya
{
	class Skul;

	class SkulHead : public GameObject
	{
	public:
		SkulHead();
		~SkulHead();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		virtual void OnCollisionEnter(class Collider* other);
		virtual void OnCollisionStay(class Collider* other);
		virtual void OnCollisionExit(class Collider* other);

		Vector2 GetPos() { return mPos; }

		virtual void SetDirect(eDirection direct) { mDirect = direct; }

	private:

		float mTime;
		Image* mLeftImage;
		Image* mRightImage;
		Vector2 mPos;
		eDirection mDirect;
	};
}
