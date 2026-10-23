#pragma once
#include "yaGameObject.h"

namespace ya
{
	class Animator;
	class Boss_Head;

	// 보스 턱. 머리를 따라다니고, 에너지볼을 모을 때 입을 벌린다.
	class Boss_Chin : public GameObject
	{
	public:
		Boss_Chin();
		~Boss_Chin();

		virtual void Initialize()override;
		virtual void Update()override;
		virtual void Render(HDC hdc)override;
		virtual void Release()override;

		// 따라갈 머리. 예전에는 Boss 레이어의 [1]번 오브젝트를 머리로 가정했다
		void SetHead(Boss_Head* head) { mHead = head; }
		void SetOpen(bool open) { mOpened = open; }
		void SetPhase2(bool phase2);

	private:
		Animator* mAnimator;
		Boss_Head* mHead;
		float mOpen;		// 0 닫힘 ~ 1 열림 (부드럽게 벌어진다)
		bool mOpened;
		bool mbPhase2;
	};
}
