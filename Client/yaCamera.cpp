#include "yaCamera.h"
#include "yaApplication.h"
#include "yaInput.h"
#include "yaTime.h"
#include "yaGameObject.h"
#include "yaTransform.h"
#include "yaImage.h"

extern ya::Application application;
namespace ya
{
	Vector2 Camera::mResolution = Vector2::Zero;
	Vector2 Camera::mLookPosition = Vector2::Zero;
	Vector2 Camera::mDistance = Vector2::Zero;
	GameObject* Camera::mTarget = nullptr;

	Camera::eCameraEffectType Camera::mType = Camera::eCameraEffectType::None;
	class Image* Camera::mCutton = nullptr;
	float Camera::mCuttonAlpha = 1.0f;
	float Camera::mAlphaTime = 0.0f;
	float Camera::mEndTime = 5.0f;


	void Camera::Initialize()
	{
		//Entity* entity = new Entity();
		//entity->SetName(L"Camera");

		mResolution.x = application.GetWidth();
		mResolution.y = application.GetHeight();
		mLookPosition = (mResolution / 2.0f);
		mType = eCameraEffectType::None;
		mCutton = Image::Create(L"Cutton", mResolution.x, mResolution.y, RGB(0, 0, 0));
	}

	void Camera::Update()
	{
		if (mTarget == nullptr)
		{

			if (Input::GetKey(eKeyCode::LEFT))
			{
				mLookPosition.x -= 400.0f * Time::DeltaTime();
			}

			if (Input::GetKey(eKeyCode::RIGHT))
			{
				mLookPosition.x += 400.0f * Time::DeltaTime();

			}

			if (Input::GetKey(eKeyCode::UP))
			{
				mLookPosition.y -= 400.0f * Time::DeltaTime();
			}

			if (Input::GetKey(eKeyCode::DOWN))
			{
				mLookPosition.y += 400.0f * Time::DeltaTime();

			}

		}

		if (mTarget != nullptr)
		{
			mLookPosition
				= mTarget->GetComponent<Transform>()->GetPos();


			if (mLookPosition.x - (mResolution.x / 2) < 0)
				mLookPosition.x = mResolution.x / 2;
		
			else if (mLookPosition.x + (mResolution.x / 2) > 3350)
				mLookPosition.x = 3350- mResolution.x / 2;


			if ((mLookPosition.y - mResolution.y / 2)<0)
			{
				mLookPosition.y = mResolution.y / 2;
			}
			else
				mLookPosition.y -= (mResolution.y / 4);
		

			mDistance = mLookPosition - (mResolution / 2.0f);
			
		}

		if (mAlphaTime < mEndTime)
		{
			//255 - > 1
			mAlphaTime += Time::DeltaTime();
			float ratio = (mAlphaTime / mEndTime);

			if (mType == eCameraEffectType::FadeIn)
			{
				mCuttonAlpha = 1.0f - ratio;
			}
			else if (mType == eCameraEffectType::FadeOut)
			{
				mCuttonAlpha = ratio;
			}
			else
			{

			}
		}


		mDistance = mLookPosition - (mResolution / 2.0f);

	}
	void Camera::Render(HDC hdc)
	{
		if (mAlphaTime < mEndTime
			&& mType == eCameraEffectType::FadeIn)
		{
			BLENDFUNCTION func = {};
			func.BlendOp = AC_SRC_OVER;
			func.BlendFlags = 0;
			func.AlphaFormat = 0;
			func.SourceConstantAlpha = (BYTE)(255.0f * mCuttonAlpha);

			AlphaBlend(hdc, 0, 0
				, mResolution.x, mResolution.y
				, mCutton->GetHdc()
				, 0, 0
				, mCutton->GetWidth(), mCutton->GetHeight()
				, func);
		}
	}

	void Camera::Clear()
	{
		mResolution.x = application.GetWidth();
		mResolution.y = application.GetHeight();
		mLookPosition = (mResolution / 2.0f);
		mDistance = Vector2::Zero;
	}
}