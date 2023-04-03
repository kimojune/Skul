#include "yaSkulHead.h"
#include "yaTransform.h"
#include "yaTime.h"
#include "yaObject.h"
#include "yaCollider.h"
#include "yaCamera.h"
#include "yaResources.h"
#include "yaSkul.h"
#include "yaPlayeScene.h"

namespace ya
{
	SkulHead::SkulHead(Skul* skul)
		:mTime(0.0f)
		, mDirect(eDirection::Left)
	{
		mSkul = skul;
		mLeftImage = Resources::Load<Image>(L"LeftSkulHead", L"..\\Resources\\LeftSkulHead.bmp");
		mRightImage = Resources::Load<Image>(L"RightSkulHead", L"..\\Resources\\SkulHead.bmp");
		Collider* collider = AddComponent<Collider>();
		collider->SetSize(Vector2(45.0f, 45.0f));
		collider->SetCenter(Vector2(0.0f, -50.0f));
	}
	SkulHead::~SkulHead()
	{
	}
	void SkulHead::Initialize()
	{
		GameObject::Initialize();



	}
	void SkulHead::Update()
	{

		if (mSkul->GetHead())
		{
			object::Destory(this);
		}
		
		Transform* tr = GetComponent<Transform>();
		Vector2 dir = Vector2(500.0f, 500.0f);
		dir.Normalize();
		/*float x = cosf(-PI / 4.0f);
		float y = sinf(-PI / 4.0f);*/
		//float x = dir.x * cosf(PI / 5.0f) - dir.y * sinf(PI / 5.0f);
		//float y = dir.x * sinf(PI / 5.0f) + dir.y * cosf(PI / 5.0f);

		

		Vector2 pos = tr->GetPos();

		if(mDirect == eDirection::Left)
			pos.x -= 1500.0f * dir.x * Time::DeltaTime();

		else if (mDirect== eDirection::Right)
			pos.x += 1500.0f * dir.x * Time::DeltaTime();

		mPos = pos;
		tr->SetPos(mPos);

		mTime += Time::DeltaTime();

		if (mTime > 4.0f)
		{
			mSkul->SetHead(true);
			object::Destory(this);
		
		}

		

		GameObject::Update();

	}
	void SkulHead::Render(HDC hdc)
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = Camera::CaluatePos(tr->GetPos());

		switch (mDirect)
		{
		case eDirection::Left:
			TransparentBlt(hdc
				, pos.x, pos.y - 50.0f
				, mLeftImage->GetWidth() * 2.8f, mLeftImage->GetHeight() * 2.8f
				, mLeftImage->GetHdc()
				, 0, 0
				, mLeftImage->GetWidth(), mLeftImage->GetHeight()
				, RGB(255, 0, 255));
			break;

		case eDirection::Right:
			TransparentBlt(hdc
				, pos.x, pos.y - 50.0f
				, mRightImage->GetWidth() * 2.8f, mRightImage->GetHeight() * 2.8f
				, mRightImage->GetHdc()
				, 0, 0
				, mRightImage->GetWidth(), mRightImage->GetHeight()
				, RGB(255, 0, 255));
			break;

		default:
			break;
		}



		GameObject::Render(hdc);
	}

	void SkulHead::Release()
	{
	}

	void SkulHead::OnCollisionEnter(Collider* other)
	{
		Skul* skul = dynamic_cast<Skul*>(other->GetOwner());


		if (skul == nullptr)
			return;

		if (mTime > 0.2f)
		{
			mSkul->SetHead(true);
			object::Destory(this);
		}
	}

	void SkulHead::OnCollisionStay(Collider* other)
	{
	}

	void SkulHead::OnCollisionExit(Collider* other)
	{
	}

}