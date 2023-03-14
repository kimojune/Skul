#pragma once
#include "yaGameObject.h"
#include "yaImage.h"
#include "yaEntity.h"


namespace ya

{ 

	class BG : public GameObject
	{
	public:
		BG(eSceneType type);
		~BG();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;
		void SetScene(eSceneType scene);



	private:
		std::vector<Image*> mImage;
		 eSceneType mScene;
		 Image* ActiveBG;
	};

}
