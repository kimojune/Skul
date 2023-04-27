#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class Boss_Chin: public GameObject
	{
	public:
		Boss_Chin();
		~Boss_Chin();

		enum class eChinState
		{
			Idle,
			Open,
			Close,
		};

		virtual void Initialize()override;
		virtual void Update()override;
		virtual void Render(HDC hdc)override;
		virtual void Release()override;
		
	private:

		void Idle();
		void Open();
		void Close();
		
	private:
		Animator* mAnimator;
		Image* mImage[3];
		float mTime;
	};

}