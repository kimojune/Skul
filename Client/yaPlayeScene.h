#pragma once
#include "yaScene.h"

namespace ya
{

	class Skul;
	class Sound;
	class TitleBG;
	class SkulHead;

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
		// 이 씬의 던지는 머리 (씬마다 하나)
		SkulHead* GetSkulHead() { return mSkulHead; }

		//Image* GetPixMap() { return mPixelMap; }
	protected:

		Sound* Chapter1;
		Sound* Chapter1_BossBG;
		Skul* mActiveSkul;
		SkulHead* mSkulHead;
	private:


	};
}
