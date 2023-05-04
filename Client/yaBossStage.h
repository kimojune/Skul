#pragma once
#include "yaPlayeScene.h"

namespace ya
{

	class BossStage : public PlayeScene
	{

	public:
		BossStage();
		~BossStage();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		virtual void OnEnter() override;
		virtual void OnExit() override;

	private:
		Sound* ElderEnt_Roar;
	};


}
