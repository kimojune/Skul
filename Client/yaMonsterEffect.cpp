#include "yaMonsterEffect.h"
#include "yaAnimator.h"
#include "yaTransform.h"
#include "yaResources.h"
#include "yaImage.h"
#include "yaTime.h"
#include "yaScene.h"
#include "yaSceneManager.h"

namespace ya
{
	// Animation::Render 는 그림의 왼쪽 위를 pos + offset - (w/2, h) 에 두고 (w x scale.x, h x scale.y) 로 그린다.
	// 배율을 키워도 pos 가 그림의 (가로 가운데, 아래 끝)에 오게 하는 offset
	static Vector2 MonsterEffectBottomAnchor(Vector2 frameSize, Vector2 scale)
	{
		return Vector2(-frameSize.x * (scale.x - 1.0f) / 2.0f, -frameSize.y * (scale.y - 1.0f));
	}

	// pos 가 그림 한가운데에 오게 하는 offset
	static Vector2 MonsterEffectCenterAnchor(Vector2 frameSize, Vector2 scale)
	{
		Vector2 offset = MonsterEffectBottomAnchor(frameSize, scale);
		offset.y += frameSize.y * scale.y / 2.0f;
		return offset;
	}

	// 애니메이션이 끝나지 않는 경우(리소스를 못 읽음 등)에도 남지 않도록 하는 최대 수명
	static constexpr float MonsterEffectMaxLifetime = 3.0f;

	MonsterEffect* MonsterEffect::Spawn(eType type, Vector2 pos)
	{
		MonsterEffect* effect = new MonsterEffect(type);
		Scene* scene = SceneManager::GetActiveScene();
		scene->AddGameObeject(effect, eLayerType::Effect);
		effect->GetComponent<Transform>()->SetPos(pos);
		effect->Initialize();
		return effect;
	}

	MonsterEffect::MonsterEffect(eType type)
		: mAnimator(nullptr)
		, mType(type)
		, mTime(0.0f)
	{
		SetName(L"MonsterEffect");
	}
	MonsterEffect::~MonsterEffect()
	{
	}
	void MonsterEffect::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		mAnimator = AddComponent<Animator>();

		switch (mType)
		{
		case eType::EntSlamSmoke:
		{
			// Land_Smoke: 21장, 가장 큰 프레임 51x17
			Vector2 scale(2.0f, 2.0f);
			tr->SetScale(scale);
			mAnimator->CreateAnimations(L"..\\Resources\\Effect\\Land_Smoke"
				, MonsterEffectBottomAnchor(Vector2(51.0f, 17.0f), scale), 0.03f);
			mAnimator->GetCompleteEvent(L"EffectLand_Smoke") = [this]() { SetState(eState::Death); };
			mAnimator->Play(L"EffectLand_Smoke", false);
			break;
		}
		case eType::EntWarning:
		{
			// Range/Loop_0: 20x99 세로 기둥. PNG를 BMP로 바꿀 때 반투명 부분이 빠져서
			// 아래쪽 붉은 빛(약 17px)만 남았다. 가로로만 넓혀(120px, 판정 폭 130과 비슷) 바닥 띠로 쓴다
			const std::wstring path = L"..\\Resources\\Effect\\Range\\Loop_0.bmp";
			Image* image = Resources::Load<Image>(path, path);
			if (image == nullptr)
				break;

			Vector2 scale(6.0f, 1.0f);
			tr->SetScale(scale);
			Vector2 size((float)image->GetWidth(), (float)image->GetHeight());
			mAnimator->CreateAnimation(L"EntWarning", image, Vector2::Zero, 1, 1, 1, 1
				, MonsterEffectBottomAnchor(size, scale), 0.1f);
			mAnimator->Play(L"EntWarning", true);
			break;
		}
		case eType::EntDeathBurst:
		{
			// Range_Attack (Explosion_Small): 29장, 가장 큰 프레임 83x86
			Vector2 scale(2.0f, 2.0f);
			tr->SetScale(scale);
			mAnimator->CreateAnimations(L"..\\Resources\\Effect\\Range_Attack"
				, MonsterEffectCenterAnchor(Vector2(83.0f, 86.0f), scale), 0.03f);
			mAnimator->GetCompleteEvent(L"EffectRange_Attack") = [this]() { SetState(eState::Death); };
			mAnimator->Play(L"EffectRange_Attack", false);
			break;
		}
		default:
			break;
		}

		GameObject::Initialize();
	}
	void MonsterEffect::Update()
	{
		mTime += (float)Time::DeltaTime();

		if (mType != eType::EntWarning && mTime > MonsterEffectMaxLifetime)
			SetState(eState::Death);

		GameObject::Update();
	}
	void MonsterEffect::Render(HDC hdc)
	{
		// 예고는 0.5초 뒤부터 깜빡여서 곧 내려친다는 것을 알린다
		if (mType == eType::EntWarning && mTime > 0.5f && ((int)(mTime * 12.0f)) % 2 == 1)
			return;

		GameObject::Render(hdc);
	}
	void MonsterEffect::Release()
	{
		GameObject::Release();
	}
}
