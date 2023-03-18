#pragma once
#include "yaGameObject.h"
#include "yaImage.h"

namespace ya
{

	class SkulHead : public GameObject
	{
	public:
		SkulHead();
		~SkulHead();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;


		virtual void SetDirect(eDirection direct) { mDirect = direct; }

	private:
		float mTime;
		Image* mLeftImage;
		Image* mRightImage;
		Vector2 mPos;
		eDirection mDirect;
	};
}
