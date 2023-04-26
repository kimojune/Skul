#pragma once

#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class Boss_Hand : public GameObject
	{
	public:
		Boss_Hand();
		~Boss_Hand();

		virtual void Initialize()override;
		virtual void Update()override;
		virtual void Render(HDC hdc)override;
		virtual void Release()override;

	private:
		Animator* mAnimator;
	};

}