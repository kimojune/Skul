#include "yaChapter1_Boss.h"
#include "yaAnimator.h"
#include "yaResources.h"
#include "yaTransform.h"
#include "yaCamera.h"
#include "yaCollider.h"
#include "yaObject.h"
#include "yaBoss_Body.h"
#include "yaBoss_Hand.h"
#include "yaBoss_Head.h"
#include "yaBossBullet.h"

namespace ya
{
	Chapter1_Boss::Chapter1_Boss()
	{
	}
	Chapter1_Boss::~Chapter1_Boss()
	{
	}
	void Chapter1_Boss::Initialize()
	{

		Transform* tr = GetComponent<Transform>();
		Vector2 bosspos = tr->GetPos();

		mBody = object::Instantiate<Boss_Body>(bosspos,eLayerType::Monster);
		mHead = object::Instantiate<Boss_Head>(bosspos,eLayerType::Monster);
		mHand[0] = object::Instantiate<Boss_Hand>(bosspos,eLayerType::Monster);
		mHand[1] = object::Instantiate<Boss_Hand>(bosspos,eLayerType::Monster);
		object::Instantiate<BossBullet>(bosspos, eLayerType::Monster);
		GameObject::Initialize();
	}
	void Chapter1_Boss::Update()
	{
		GameObject::Update();
	}
	void Chapter1_Boss::Render(HDC hdc)
	{

		
	}
	void Chapter1_Boss::Release()
	{
		GameObject::Release();

	}
}
