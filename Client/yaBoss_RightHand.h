#pragma once
#include "yaBoss_Hand.h"

namespace ya
{
	// 보스 오른손. 동작은 Boss_Hand와 같고 그림만 오른손 것을 쓴다.
	// (예전에는 Boss_Hand를 통째로 복사한 클래스였다)
	class Boss_RightHand : public Boss_Hand
	{
	public:
		Boss_RightHand();
		virtual ~Boss_RightHand();
	};
}
