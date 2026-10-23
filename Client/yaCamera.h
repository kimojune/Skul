#pragma once

#include "YamYamEngine.h"

namespace ya
{ 
	class GameObject;
	class Camera
	{
	public:

		enum class eCameraEffectType
		{
			None,
			FadeIn,
			FadeOut,
			End,
		};

		static void Initialize();
		static void Update();
		static void Render(HDC hdc);
		static void Clear();

		static void SetTarget(GameObject* target) { mTarget = target; }
		static Vector2 CaluatePos(Vector2 pos) { return pos - mDistance; }
		static Vector2 GetLookPosition() { return mLookPosition; }

		// 타겟을 따라갈 때 화면이 이 월드 사각형 밖을 보지 않게 한다. Clear()에서 경계 없음으로 돌아간다.
		static void SetBounds(float left, float top, float right, float bottom);
		static void ClearBounds();

		// intensity(px) 만큼 흔들고 duration(초) 동안 점점 약해진다. 그리는 위치만 바뀐다.
		static void Shake(float intensity, float duration);

		// FadeIn: 검은 화면에서 밝아진다. FadeOut: 검게 어두워지고, 다음 FadeIn 까지 검은 화면을 유지한다.
		static void FadeIn(float duration);
		static void FadeOut(float duration);

	private:
		static Vector2 mResolution;
		static Vector2 mLookPosition;
		static Vector2 mDistance;
		static GameObject* mTarget;

		static float mBoundLeft;
		static float mBoundTop;
		static float mBoundRight;
		static float mBoundBottom;

		static float mShakeIntensity;
		static float mShakeDuration;
		static float mShakeTime;

		static eCameraEffectType mType;
		static class Image* mCutton;
		static float mCuttonAlpha;
		static float mAlphaTime;
		static float mEndTime;

	};

}
