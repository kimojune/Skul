#pragma once
#include "yaComponent.h"
#include "yaAnimation.h"
#include "yaImage.h"

namespace ya
{
	class Animator : public Component
	{
	public:
		struct Event
		{
			void operator=(std::function<void()> func)
			{
				mEvent = std::move(func);
			}
			void operator()()
			{
				if (mEvent)
					mEvent();
			}
			
			std::function<void()> mEvent;
		};

		struct Events
		{
			Event mStartEvent;
			Event mCompleteEvent;
			Event mEndEvent;
		};

		Animator();
		~Animator();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Render(HDC hdc) override;
		virtual void Release() override;

		//완성된 스프라이트 시트를 지정해서 애니메이션 제작하는 함수
		void CreateAnimation(const std::wstring& name
			, Image* sheet
			, Vector2 leftTop ,int next
			, UINT coulmn, UINT row, UINT spriteLength
			, Vector2 offset, float duration);

		// 폴더에 들어있는 스프라이트들을 이용해서 애니메이션 제작해주는 함수
		void CreateAnimations(const std::wstring& path, Vector2 offset, float duration);

		Animation* FindAnimation(const std::wstring& name);
		void Play(const std::wstring& name, bool loop);

		Events* FindEvents(const std::wstring& name);
		std::function<void()>& GetStartEvent(const std::wstring& name);
		std::function<void()>& GetCompleteEvent(const std::wstring& name);
		std::function<void()>& GetEndEvent(const std::wstring& name);

		Animation* GetAnimaiton() { return mActiveAnimation; }

		// 좌우 반전과 전체 투명도. 지금 재생 중인 애니메이션과 이후에 재생하는 애니메이션 모두에 적용된다.
		// 반전은 프레임 안에서만 뒤집고 offset 은 그대로 쓴다. 이 게임의 왼쪽 시트(SkulLeft.bmp 등)는
		// 오른쪽 시트를 뒤집은 그림에 같은 offset 을 쓰므로, Right 애니메이션 + SetFlipX(true) 가 Left 애니메이션과 같게 그려진다
		void SetFlipX(bool flip) { mbFlipX = flip; }
		bool IsFlipX() { return mbFlipX; }
		void SetAlpha(BYTE alpha) { mAlpha = alpha; }	// 0(투명) ~ 255(불투명)
		BYTE GetAlpha() { return mAlpha; }

		// 흰색으로 번쩍임 (0 없음 ~ 255 완전히 흰색). DX11만 그린다. GDI 는 번쩍이지 않는다.
		// FlashFor 는 duration(초, 시간 배율과 무관한 실제 시간) 동안 완전히 흰색으로 그린다
		void SetFlash(BYTE flash) { mFlash = flash; }
		void FlashFor(float duration);
		BYTE GetFlash() { return (mFlash > 0 || mFlashTime <= 0.0f) ? mFlash : 255; }
		bool IsFlashing() { return mFlashTime > 0.0f; }
		// 지금 번쩍이는 Animator 수 (시나리오 fx.flashing)
		static int GetFlashingCount() { return mFlashingCount; }

	private:
		std::map<std::wstring, Animation*> mAnimations;
		std::map<std::wstring, Events*> mEvents;
		Animation* mActiveAnimation;
		Image* mSpriteSheet;
		bool mbLoop;
		bool mbFlipX;
		BYTE mAlpha;
		BYTE mFlash;
		float mFlashTime;
		static int mFlashingCount;
	};
}
