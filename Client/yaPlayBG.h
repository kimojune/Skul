#pragma once
#include "yaGameObject.h"
#include "yaImage.h"
#include "yaEntity.h"


namespace ya
	
{ 
	class PlayBG : public GameObject
	{
	public:
		PlayBG();
		~PlayBG();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

	private:
		Image* mImage;
	};

}
