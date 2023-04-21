#pragma once
#include "yaScene.h"

namespace ya
{

	class Skul;
	class Sound;
	class TitleBG;

	extern Skul* mSkuls[(UINT)Skul::eSkulType::End];

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
		
		//Image* GetPixMap() { return mPixelMap; }
	protected:

		Skul* mActiveSkul;
	private:
		Sound* Chapter1;


	};
}
