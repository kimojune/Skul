#pragma once

#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class Image;
	class Boss_Hand : public GameObject
	{
	public:
		Boss_Hand();
		Boss_Hand(bool isRight);
		~Boss_Hand();

		enum class eHandState
		{
			Idle,
			HandsUp,
			Attack1,
			Attack2,
			Attack3,
		};

		virtual void Initialize()override;
		virtual void Update()override;
		virtual void Render(HDC hdc)override;
		virtual void Release()override;

	private:
		Animator* mAnimator;
		Image* mImage[3];
		eDirection mDirection;
	};

}