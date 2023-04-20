#include "yaAnimator.h"
#include "yaResources.h"


namespace ya
{
	Animator::Animator()
		: Component(eComponentType::Animator)
		, mActiveAnimation(nullptr)
		, mSpriteSheet(nullptr)
		, mbLoop(false)
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
			mActiveAnimation->Update();

			if (mActiveAnimation->IsComplete())
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
		std::vector<Image*> images = {};
		for (const auto& p : std::filesystem::recursive_directory_iterator(path))
		{
			std::wstring fileName = p.path().filename();
			std::wstring fullName = path + L"\\" + fileName;

			const std::wstring ext = p.path().extension();
			if (ext == L".png")
				continue;

			Image* image = Resources::Load<Image>(fileName, fullName);
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

		//std::wstring key = fs.parent_path().filename();
		std::wstring key = fs.filename();
		/*key += fs.filename();*/

		mSpriteSheet = Image::Create(key, width * fileCount, height);

		HBRUSH magentaBrush = CreateSolidBrush(RGB(255, 0, 255));
		HBRUSH oldBrush = (HBRUSH)SelectObject(mSpriteSheet->GetHdc(), magentaBrush);
		Rectangle(mSpriteSheet->GetHdc(), -1, -2, width * fileCount + 2, height + 1);
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

			SelectObject(mSpriteSheet->GetHdc(), oldBrush);
			index++;
		}
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
		if (mActiveAnimation != nullptr)
		{
			Animator::Events* prevEvents
				= FindEvents(mActiveAnimation->GetAnimationName());

			if (prevEvents != nullptr)
				prevEvents->mEndEvent();
		}

		mActiveAnimation = FindAnimation(name);
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
	std::function<void()>& Animator::GetStartEvent(const std::wstring& name)
	{
		Animation* animation = FindAnimation(name);

		Animator::Events* events
			= FindEvents(animation->GetAnimationName());

		return events->mStartEvent.mEvent;
	}
	std::function<void()>& Animator::GetCompleteEvent(const std::wstring& name)
	{
		Animation* animation = FindAnimation(name);

		Animator::Events* events
			= FindEvents(animation->GetAnimationName());

		return events->mCompleteEvent.mEvent;
	}
	std::function<void()>& Animator::GetEndEvent(const std::wstring& name)
	{
		Animation* animation = FindAnimation(name);

		Animator::Events* events
			= FindEvents(animation->GetAnimationName());

		return events->mEndEvent.mEvent;
	}
}