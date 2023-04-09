#pragma once
#include "yaScene.h"

namespace ya
{
	class Player;
	class Sound;
	class TitleBG;
	class PlayeScene : public Scene
	{
	public:
		PlayeScene();
		~PlayeScene();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		virtual void OnEnter() override;
		virtual void OnExit() override;
		Player* GetPlayer() { return mPlayer; }

	private:
		Sound* Chapter1;
		Player* mPlayer;
	};
}
