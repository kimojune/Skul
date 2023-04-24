#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Platform : public GameObject
	{
	public:
		Platform();
		virtual ~Platform();

		virtual void Initialize();
		virtual void Update();
		virtual void Render(HDC hdc);
		virtual void Release();

		virtual void OnCollisionEnter(class Collider* other);
		virtual void OnCollisionStay(class Collider* other);
		virtual void OnCollisionExit(class Collider* other);

		void SetPlayer(class Skul* player) { mPlayer = player; }

	private:
		class Collider* mCollider;
		class Image* mImage;
		class Skul* mPlayer;
	};
}