#include "yaCamera.h"
#include "yaApplication.h"
#include "yaInput.h"
#include "yaTime.h"
#include "yaGameObject.h"
#include "yaTransform.h"
#include "yaImage.h"
#include <cfloat>

extern ya::Application application;
namespace ya
{
	Vector2 Camera::mResolution = Vector2::Zero;
	Vector2 Camera::mLookPosition = Vector2::Zero;
	Vector2 Camera::mDistance = Vector2::Zero;
	GameObject* Camera::mTarget = nullptr;

	// 기본은 경계 없음. 플레이 씬은 OnEnter 에서 SetBounds 를 부른다.
	float Camera::mBoundLeft = -FLT_MAX;
	float Camera::mBoundTop = -FLT_MAX;
	float Camera::mBoundRight = FLT_MAX;
	float Camera::mBoundBottom = FLT_MAX;

	float Camera::mShakeIntensity = 0.0f;
	float Camera::mShakeDuration = 0.0f;
	float Camera::mShakeTime = 0.0f;

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

			// 좌우 경계: 화면이 [left, right] 밖을 보지 않게
			if (mLookPosition.x - (mResolution.x / 2) < mBoundLeft)
				mLookPosition.x = mBoundLeft + mResolution.x / 2;

			else if (mLookPosition.x + (mResolution.x / 2) > mBoundRight)
				mLookPosition.x = mBoundRight - mResolution.x / 2;

			// 위쪽 경계는 화면을 해상도/4 만큼 올리기 전 좌표로 검사한다 (기존 동작 그대로).
			// 그래서 타겟이 위쪽에 있으면 화면 위가 top 보다 최대 해상도/4 만큼 더 올라갈 수 있다.
			if ((mLookPosition.y - mResolution.y / 2) < mBoundTop)
			{
				mLookPosition.y = mBoundTop + mResolution.y / 2;
			}
			else
				mLookPosition.y -= (mResolution.y / 4);

			// 아래쪽 경계: 화면 아래가 bottom 을 넘지 않게
			if (mLookPosition.y + (mResolution.y / 2) > mBoundBottom)
				mLookPosition.y = mBoundBottom - mResolution.y / 2;
		}

		// 페이드: FadeIn 은 다 밝아지면 끝, FadeOut 은 다 어두워진 뒤에도 검은 화면 유지
		if (mType == eCameraEffectType::FadeIn
			|| mType == eCameraEffectType::FadeOut)
		{
			mAlphaTime += (float)Time::DeltaTime();
			float ratio = (mEndTime > 0.0f) ? (mAlphaTime / mEndTime) : 1.0f;
			if (ratio > 1.0f)
				ratio = 1.0f;

			if (mType == eCameraEffectType::FadeIn)
			{
				mCuttonAlpha = 1.0f - ratio;
				if (ratio >= 1.0f)
					mType = eCameraEffectType::None;
			}
			else
			{
				mCuttonAlpha = ratio;
			}
		}

		mDistance = mLookPosition - (mResolution / 2.0f);

		// 흔들기는 그리는 위치(mDistance)에만 더한다. mLookPosition 은 그대로.
		// rand()를 쓰면 게임 쪽 rand() 순서(보스 패턴 등)가 바뀌므로 sin/cos 로 흔든다.
		if (mShakeTime < mShakeDuration)
		{
			mShakeTime += (float)Time::DeltaTime();

			float power = mShakeIntensity * (1.0f - mShakeTime / mShakeDuration);
			if (power < 0.0f)
				power = 0.0f;

			mDistance.x += power * sinf(mShakeTime * 97.0f);
			mDistance.y += power * cosf(mShakeTime * 71.0f);
		}
	}
	void Camera::Render(HDC hdc)
	{
		if (mType == eCameraEffectType::None
			|| mCutton == nullptr)
			return;

		float alpha = mCuttonAlpha;
		if (alpha < 0.0f)
			alpha = 0.0f;
		if (alpha > 1.0f)
			alpha = 1.0f;

		BYTE sourceAlpha = (BYTE)(255.0f * alpha);
		if (sourceAlpha == 0)
			return;

		BLENDFUNCTION func = {};
		func.BlendOp = AC_SRC_OVER;
		func.BlendFlags = 0;
		func.AlphaFormat = 0;
		func.SourceConstantAlpha = sourceAlpha;

		AlphaBlend(hdc, 0, 0
			, (int)mResolution.x, (int)mResolution.y
			, mCutton->GetHdc()
			, 0, 0
			, mCutton->GetWidth(), mCutton->GetHeight()
			, func);
	}

	void Camera::Clear()
	{
		mResolution.x = application.GetWidth();
		mResolution.y = application.GetHeight();
		mLookPosition = (mResolution / 2.0f);
		mDistance = Vector2::Zero;

		// 씬이 바뀌면 경계와 흔들기는 초기화. 페이드는 씬 전환에 걸쳐 이어지도록 그대로 둔다.
		ClearBounds();
		mShakeTime = 0.0f;
		mShakeDuration = 0.0f;
	}

	void Camera::SetBounds(float left, float top, float right, float bottom)
	{
		mBoundLeft = left;
		mBoundTop = top;
		mBoundRight = right;
		mBoundBottom = bottom;
	}

	void Camera::ClearBounds()
	{
		SetBounds(-FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX);
	}

	void Camera::Shake(float intensity, float duration)
	{
		mShakeIntensity = intensity;
		mShakeDuration = duration;
		mShakeTime = 0.0f;
	}

	void Camera::FadeIn(float duration)
	{
		mType = eCameraEffectType::FadeIn;
		mEndTime = duration;
		mAlphaTime = 0.0f;
		mCuttonAlpha = 1.0f;
	}

	void Camera::FadeOut(float duration)
	{
		mType = eCameraEffectType::FadeOut;
		mEndTime = duration;
		mAlphaTime = 0.0f;
		mCuttonAlpha = 0.0f;
	}
}