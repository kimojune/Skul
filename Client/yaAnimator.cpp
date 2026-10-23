#include "yaAnimator.h"
#include "yaResources.h"
#include <algorithm>


namespace ya
{
	static bool AnimatorIsDigit(wchar_t ch)
	{
		return L'0' <= ch && ch <= L'9';
	}

	// 프레임 파일명 비교. 숫자 구간은 값으로 비교해서 _2 가 _10 보다 앞에 온다.
	// 나머지 글자는 대소문자를 무시하고 비교한다.
	static bool AnimatorFrameNameLess(const std::wstring& left, const std::wstring& right)
	{
		size_t i = 0;
		size_t j = 0;
		while (i < left.size() && j < right.size())
		{
			if (AnimatorIsDigit(left[i]) && AnimatorIsDigit(right[j]))
			{
				size_t leftEnd = i;
				while (leftEnd < left.size() && AnimatorIsDigit(left[leftEnd]))
					leftEnd++;
				size_t rightEnd = j;
				while (rightEnd < right.size() && AnimatorIsDigit(right[rightEnd]))
					rightEnd++;

				// 앞자리 0은 건너뛰고 자릿수 -> 숫자 순으로 비교
				size_t leftStart = i;
				while (leftStart + 1 < leftEnd && left[leftStart] == L'0')
					leftStart++;
				size_t rightStart = j;
				while (rightStart + 1 < rightEnd && right[rightStart] == L'0')
					rightStart++;

				size_t leftLength = leftEnd - leftStart;
				size_t rightLength = rightEnd - rightStart;
				if (leftLength != rightLength)
					return leftLength < rightLength;

				int compare = left.compare(leftStart, leftLength, right, rightStart, rightLength);
				if (compare != 0)
					return compare < 0;

				i = leftEnd;
				j = rightEnd;
				continue;
			}

			wchar_t leftChar = towupper(left[i]);
			wchar_t rightChar = towupper(right[j]);
			if (leftChar != rightChar)
				return leftChar < rightChar;

			i++;
			j++;
		}

		size_t leftRest = left.size() - i;
		size_t rightRest = right.size() - j;
		if (leftRest != rightRest)
			return leftRest < rightRest;

		// "01" 과 "1" 처럼 같은 값이면 원래 문자열 순서로 고정
		return left < right;
	}

	// 같은 메시지는 한 번만 디버그 출력에 남긴다 (매 프레임 Play 해도 한 줄).
	static void AnimatorLogOnce(const std::wstring& message)
	{
		static std::map<std::wstring, bool> loggedMessages;
		if (loggedMessages.find(message) != loggedMessages.end())
			return;

		loggedMessages[message] = true;
		OutputDebugStringW((L"[Animator] " + message + L"\n").c_str());
	}

	// 없는 애니메이션의 이벤트를 요청하면 아무 데도 연결되지 않은 빈 이벤트를 돌려준다.
	static std::function<void()>& AnimatorUnknownEvent(const wchar_t* where, const std::wstring& name)
	{
		AnimatorLogOnce(std::wstring(where) + L": unknown animation '" + name + L"'");

		static std::function<void()> unusedEvent;
		unusedEvent = nullptr;
		return unusedEvent;
	}

	Animator::Animator()
		: Component(eComponentType::Animator)
		, mActiveAnimation(nullptr)
		, mSpriteSheet(nullptr)
		, mbLoop(false)
		, mbFlipX(false)
		, mAlpha(255)
	{

	}
	Animator::~Animator()
	{
		for (auto animation : mAnimations)
		{
			delete animation.second;
			animation.second = nullptr;
		}
		for (auto events : mEvents)
		{
			delete events.second;
			events.second = nullptr;
		}
	}
	void Animator::Initialize()
	{
		/*HDC mSpriteHdc = mSpriteSheet->GetHdc();

		HBRUSH magentaBrush = CreateSolidBrush(RGB(255, 0, 255));
		HBRUSH oldBrush = (HBRUSH)SelectObject(mSpriteHdc, magentaBrush);
		Rectangle(mSpriteHdc, -1, -1, 1602, 902);
		SelectObject(mSpriteHdc, oldBrush);
		DeleteObject(magentaBrush);*/
	}
	void Animator::Update()
	{
		if (mActiveAnimation)
		{
			// 완료 이벤트는 끝나는 순간에 한 번만 부른다.
			// 반복 재생이 아니면 끝난 뒤에도 IsComplete()가 계속 true라서 매 프레임 불리던 것을 막는다.
			// 반복 재생은 아래에서 Reset 되므로 한 바퀴마다 한 번씩 불린다.
			bool wasComplete = mActiveAnimation->IsComplete();
			mActiveAnimation->Update();

			if (!wasComplete && mActiveAnimation->IsComplete())
			{
				Animator::Events* events
					= FindEvents(mActiveAnimation->GetAnimationName());

				if (events != nullptr)
					events->mCompleteEvent();
			}
			if (mbLoop == true && mActiveAnimation->IsComplete())
			{
				mActiveAnimation->Reset();
			}
		}
	}
	void Animator::Render(HDC hdc)
	{
		if (mActiveAnimation)
		{
			mActiveAnimation->Render(hdc);
		}
	}
	void Animator::Release()
	{
	}
	void Animator::CreateAnimation(const std::wstring& name
		, Image* sheet, Vector2 leftTop, int next
		, UINT coulmn, UINT row, UINT spriteLength
		, Vector2 offset, float duration)
	{
		Animation* animation = FindAnimation(name);


		if (animation != nullptr)
			return;

		animation = new Animation();
		animation->Create(sheet, leftTop, next, coulmn, row, spriteLength, offset, duration);
		animation->SetAnimationName(name);
		animation->SetAnimator(this);

		mAnimations.insert(std::make_pair(name, animation));
		Events* event = new Events();
		mEvents.insert(std::make_pair(name, event));
	}

	void Animator::CreateAnimations(const std::wstring& path, Vector2 offset, float duration)
	{
		UINT width = 0;
		UINT height = 0;
		UINT fileCount = 0;


		std::filesystem::path fs(path);

		// 디렉터리 순서는 이름순이라 _10 이 _2 보다 먼저 나온다.
		// .bmp 파일 이름만 모아서 숫자 순으로 정렬한 뒤 읽는다.
		std::error_code error;
		std::filesystem::directory_iterator directory(path, error);
		if (error)
		{
			AnimatorLogOnce(L"CreateAnimations: cannot open folder '" + path + L"'");
			return;
		}

		std::vector<std::wstring> fileNames = {};
		for (const auto& p : directory)
		{
			if (!p.is_regular_file())
				continue;

			const std::wstring ext = p.path().extension();
			if (_wcsicmp(ext.c_str(), L".bmp") != 0)
				continue;

			fileNames.push_back(p.path().filename());
		}
		std::sort(fileNames.begin(), fileNames.end(), AnimatorFrameNameLess);

		std::vector<Image*> images = {};
		for (const std::wstring& fileName : fileNames)
		{
			std::wstring fullName = path + L"\\" + fileName;

			// 다른 폴더에 같은 파일명이 많아서 캐시 키는 전체 경로로 쓴다.
			Image* image = Resources::Load<Image>(fullName, fullName);
			images.push_back(image);

			if (width < image->GetWidth())
			{
				width = image->GetWidth();
			}
			if (height < image->GetHeight())
			{
				height = image->GetHeight();
			}
			fileCount++;
		}

		if (fileCount == 0)
		{
			AnimatorLogOnce(L"CreateAnimations: no .bmp frames in '" + path + L"'");
			return;
		}

		// 애니메이션 이름은 "상위폴더명 + 폴더명" (예: BulletLoop)
		std::wstring key = fs.parent_path().filename();
		//std::wstring key = fs.filename();
		key += fs.filename();

		// 합친 시트 이미지는 이름이 겹치지 않도록 폴더 전체 경로를 키로 쓴다.
		mSpriteSheet = Image::Create(path, width * fileCount, height);

		HBRUSH magentaBrush = CreateSolidBrush(RGB(255, 0, 255));
		HBRUSH oldBrush = (HBRUSH)SelectObject(mSpriteSheet->GetHdc(), magentaBrush);
		Rectangle(mSpriteSheet->GetHdc(), -1, -2, width * fileCount + 2, height + 1);
		// 선택된 채로는 지워지지 않으므로 원래 브러시를 먼저 되돌린다.
		SelectObject(mSpriteSheet->GetHdc(), oldBrush);
		DeleteObject(magentaBrush);

		int index = 0;
		for (Image* image : images)
		{
			int centerX = (width - image->GetWidth()) / 2;
			int centerY = (height - image->GetHeight());

		
			BitBlt(mSpriteSheet->GetHdc()
				, width * index + centerX
				, 0 + centerY
				, image->GetWidth(), image->GetHeight()
				, image->GetHdc(), 0, 0, SRCCOPY);

			index++;
		}
		// GDI 로 직접 그렸으므로 GPU 텍스처가 있다면 다시 올리게 한다
		mSpriteSheet->MarkModified();
		CreateAnimation(key, mSpriteSheet, Vector2::Zero,1, index, 1, index, offset, duration);
	}

	Animation* Animator::FindAnimation(const std::wstring& name)
	{
		std::map<std::wstring, Animation*>::iterator iter
			= mAnimations.find(name);

		if (iter == mAnimations.end())
			return nullptr;

		return iter->second;
	}
	void Animator::Play(const std::wstring& name, bool loop)
	{
		Animation* animation = FindAnimation(name);
		if (animation == nullptr)
		{
			// 없는 이름이면 아무것도 바꾸지 않는다 (지금 애니메이션과 이벤트 그대로)
			AnimatorLogOnce(L"Play: unknown animation '" + name + L"'");
			return;
		}

		if (mActiveAnimation != nullptr)
		{
			Animator::Events* prevEvents
				= FindEvents(mActiveAnimation->GetAnimationName());

			if (prevEvents != nullptr)
				prevEvents->mEndEvent();
		}

		mActiveAnimation = animation;
		mActiveAnimation->Reset();
		mbLoop = loop;

		if (!mbLoop)
		{
			//mActiveAnimation->Reset();
		}

		Animator::Events* events
			= FindEvents(mActiveAnimation->GetAnimationName());

		if (events != nullptr)
			events->mStartEvent();

	}
	Animator::Events* Animator::FindEvents(const std::wstring& name)
	{
		std::map<std::wstring, Events*>::iterator iter
			= mEvents.find(name);

		if (iter == mEvents.end())
			return nullptr;

		return iter->second;
	}
	// 이벤트는 CreateAnimation 에서 애니메이션과 같은 이름으로 만들어진다.
	std::function<void()>& Animator::GetStartEvent(const std::wstring& name)
	{
		Animator::Events* events = FindEvents(name);
		if (events == nullptr)
			return AnimatorUnknownEvent(L"GetStartEvent", name);

		return events->mStartEvent.mEvent;
	}
	std::function<void()>& Animator::GetCompleteEvent(const std::wstring& name)
	{
		Animator::Events* events = FindEvents(name);
		if (events == nullptr)
			return AnimatorUnknownEvent(L"GetCompleteEvent", name);

		return events->mCompleteEvent.mEvent;
	}
	std::function<void()>& Animator::GetEndEvent(const std::wstring& name)
	{
		Animator::Events* events = FindEvents(name);
		if (events == nullptr)
			return AnimatorUnknownEvent(L"GetEndEvent", name);

		return events->mEndEvent.mEvent;
	}
}