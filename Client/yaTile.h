#pragma once
#include "yaGameObject.h"
#include "yaImage.h"

namespace ya
{
	class Tile : public GameObject
	{
	public:
		Tile();
		Tile(Vector2 pos);
		~Tile();

		void InitializeTile(Image* atlas, int index);
		void SetIndex(int index);

		virtual void Update() override;
		virtual void Render(HDC hdc) override;

		UINT Y() { return mY; }
		UINT X() { return mX; }
		UINT Index() { return mIndex; }
		
		virtual void OnCollisionEnter(class Collider* other);
		virtual void OnCollisionStay(class Collider* other);
		virtual void OnCollisionExit(class Collider* other);

		

	private:
		Image* mAtlas;
		UINT mIndex;
		UINT mX;
		UINT mY;
	};

}
