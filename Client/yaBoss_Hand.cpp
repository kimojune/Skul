#include "yaBoss_Hand.h"

namespace ya
{
	Boss_Hand::Boss_Hand()
	{
	}
	Boss_Hand::~Boss_Hand()
	{
	}
	void Boss_Hand::Initialize()
	{
		mAnimator = AddComponent<Animator>();
		GameObject::Initialize();
	}
	void Boss_Hand::Update()
	{
	}
	void Boss_Hand::Render(HDC hdc)
	{
	}
	void Boss_Hand::Release()
	{
	}
}