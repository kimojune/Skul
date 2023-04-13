#pragma once
#include "yaScene.h"

namespace ya
{
	extern Skul* mSkuls[(UINT)Skul::eSkulType::End];

	class Skul;
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
		
		
		virtual void SetSkul(Skul::eSkulType type);
		Skul* GetSkul() { return mActiveSkul;}
		
		
		

	private:
		Sound* Chapter1;
		Skul* mActiveSkul;

	};
}
