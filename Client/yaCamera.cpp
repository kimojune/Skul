#include "yaCamera.h"
#include "yaApplication.h"
#include "yaInput.h"
#include "yaTime.h"
#include "yaGameObject.h"
#include "yaTransform.h"

extern ya::Application application;
namespace ya
{
	Vector2 Camera::mResolution = Vector2::Zero;
	Vector2 Camera::mLookPosition = Vector2::Zero;
	Vector2 Camera::mDistance = Vector2::Zero;
	GameObject* Camera::mTarget = nullptr;

	void Camera::Initialize()
	{
		mResolution.x = application.GetWidth();
		mResolution.y = application.GetHeight();
		mLookPosition = (mResolution / 2.0f);
	}

	void Camera::Update()
	{

		if (Input::GetKey(eKeyCode::LEFT))
		{
			mLookPosition.x -= 200.0f * Time::DeltaTime();
		}

		if (Input::GetKey(eKeyCode::RIGHT))
		{
			mLookPosition.x += 200.0f * Time::DeltaTime();

		}

		if (mTarget != nullptr)
		{
			mLookPosition
				= mTarget->GetComponent<Transform>()->GetPos();
		}

		mDistance = mLookPosition - (mResolution / 2.0f);
	}
}