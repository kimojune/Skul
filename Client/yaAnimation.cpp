#include "yaAnimation.h"
#include "yaTime.h"
#include "yaImage.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaGameObject.h"
#include "yaCamera.h"
#include "yaRenderer.h"

namespace ya
{
    Animation::Animation()
        : mAnimator(nullptr)
        , mSheetImage(nullptr)
        , mTime(0.0f)
        , mbComplete(false)
        , mSpriteIndex(0)
        , mAnimationName(L"")
    {
        
    }

    Animation::~Animation()
    {
    }

    void Animation::Initialize()
    {
    }

    void Animation::Update()
    {
        if (mbComplete == true)
            return;

        mTime += Time::DeltaTime();
        if (mSpriteSheet[mSpriteIndex].duration < mTime)
        {
            mTime = 0.0f;

            if (mSpriteSheet.size() <= mSpriteIndex + 1)
            {
                mbComplete = true;
            }
            else
            {
                mSpriteIndex++;
            }
        }
    }

    void Animation::Render(HDC hdc)
    {
        Transform* tr 
            = mAnimator->GetOwner()->GetComponent<Transform>();
        Vector2 scale = tr->GetScale();

        // 이미지가 그려질 좌표는 오브젝트 좌표의 위쪽 중간에 그려진다.
        // 캐릭터의 발을 기준으로 포지션을 계산
        Vector2 pos = tr->GetPos();
        pos = Camera::CaluatePos(pos);
        pos += mSpriteSheet[mSpriteIndex].offset;
        pos.x -= mSpriteSheet[mSpriteIndex].size.x / 2.0f;
        pos.y -= mSpriteSheet[mSpriteIndex].size.y;

        // 좌표와 크기는 GDI 때와 같이 소수점 아래를 버린 정수 픽셀로 넘긴다
        const Sprite& sprite = mSpriteSheet[mSpriteIndex];
        Renderer::DrawImage(mSheetImage
            , (int)pos.x, (int)pos.y
            , (int)(sprite.size.x * scale.x), (int)(sprite.size.y * scale.y)
            , (int)sprite.leftTop.x, (int)sprite.leftTop.y
            , (int)sprite.size.x, (int)sprite.size.y);
    }

    void Animation::Create(Image* sheet, Vector2 leftTop
        ,int next, UINT coulmn, UINT row, UINT spriteLength
        , Vector2 offset, float duration)
    {
        mSheetImage = sheet;

        //UINT coulmn = mSheetImage->GetWidth() / size.x;
        Vector2 size = Vector2::One;
        size.x = mSheetImage->GetWidth() / (float)coulmn;
        size.y = mSheetImage->GetHeight() / (float)row;

        for (size_t i = 0; i < spriteLength; i++)
        {
            Sprite spriteInfo;

            spriteInfo.leftTop.x = leftTop.x + (size.x * i * next);
            spriteInfo.leftTop.y = leftTop.y;
            spriteInfo.size = size;
            spriteInfo.offset = offset;
            spriteInfo.duration = duration;

            mSpriteSheet.push_back(spriteInfo);
        }
    }

    void Animation::Reset()
    {
        mSpriteIndex = 0;
        mTime = 0.0f;
        mbComplete = false;
    }

}
