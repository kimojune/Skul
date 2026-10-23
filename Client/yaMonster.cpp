#include "yaMonster.h"
#include "yaTime.h"
#include "yaSceneManager.h"
#include "yaInput.h"
#include "yaResources.h"
#include "yaTransform.h"
#include "yaAnimator.h"
#include "yaCollider.h"
#include "yaSkulHead.h"
#include "yaScene.h"
#include "yaRigidbody.h"
#include "yaPlayeScene.h"
#include "yaSkul.h"
#include "yaSound.h"
#include "yaCamera.h"
#include "yaImage.h"
#include "yaScenario.h"
#include "yaPlayerStats.h"
#include "yaMonsterEffect.h"
#include "yaEntSlamHitbox.h"


namespace ya
{
	// 엔트 수치. 원작 1장 엔트: 주먹을 들었다 내려침, 느림, 플레이어가 받는 피해 10 이상
	// (docs/ai-rework/04-original-reference.md). 시간은 초, 거리는 월드 px.
	// 거리 기준점: 엔트는 그림 가운데(Transform x + BodyCenterX), 플레이어는 충돌체 가운데.
	namespace EntTuning
	{
		constexpr int MaxHP = 6;					// 스컬 공격 1타 = 1. 2타 콤보 3번이면 쓰러진다
		constexpr int SlamDamage = 10;

		constexpr float WanderSpeed = 60.0f;		// 스컬 걷기는 200
		constexpr float ChaseSpeed = 100.0f;
		constexpr float WanderInterval = 3.0f;		// 배회: 3초마다 멈춤/걷기와 방향을 무작위로 고른다 (원래 동작)
		constexpr float WanderLeash = 200.0f;		// 배회는 배치된 x에서 이 거리 안에서만

		constexpr float DetectRangeX = 500.0f;		// 이 안에 들어오면 쫓기 시작
		constexpr float DetectRangeY = 200.0f;
		constexpr float LoseRangeX = 700.0f;		// 이 밖으로 나가면 포기
		constexpr float LoseRangeY = 400.0f;
		constexpr float AttackRangeX = 110.0f;		// 여기까지 다가가서 멈추고 내려친다
		constexpr float AttackRangeY = 150.0f;		// 플레이어가 이보다 높이 있으면(발판 위 등) 내려치지 않는다
		constexpr float FaceDeadZone = 10.0f;		// 거의 겹쳤을 때 좌우로 뒤집히지 않게

		constexpr float WindupTime = 0.9f;			// 예고: 주먹을 든 자세 + 바닥의 붉은 경고
		constexpr float WindupShakeStart = 0.3f;	// 들어 올린 뒤 이때부터 떤다
		constexpr float WindupShakeAmount = 3.0f;
		constexpr float RecoverTime = 0.6f;		// 내려친 자세로 멈춰 있는 시간
		constexpr float SlamCooldown = 1.6f;		// 내려친 순간부터 다음 예고까지 최소 간격
		constexpr float HitStunTime = 0.3f;		// 피격 경직 (원래 값)
		constexpr float KnockbackSpeed = 60.0f;	// 피격 경직 동안 밀려나는 속도 (원래 20)
		constexpr float DyingTime = 0.35f;			// 마지막 피격 자세로 멈췄다가 폭발하며 사라진다

		// 그림 범위 (Transform 기준, 배율 2): x -42 ~ +70, y -74 ~ +58
		constexpr float BodyCenterX = 14.0f;
		constexpr float BodyLeft = -42.0f;
		constexpr float BodyRight = 70.0f;
		constexpr float BodyCenterY = -8.0f;
		constexpr float GroundY = 52.0f;			// 발이 닿는 바닥
		constexpr float FistReach = 85.0f;			// 몸 가운데에서 주먹이 떨어지는 지점까지
		constexpr float SlamWidth = 130.0f;		// 내려치기 판정 (바닥에서 위로)
		constexpr float SlamHeight = 140.0f;
		constexpr float SlamActiveTime = 0.1f;

		constexpr float MapLeft = 0.0f;			// 스테이지 카메라 경계 (Stage1/2 OnEnter 의 Camera::SetBounds)
		constexpr float MapRight = 3350.0f;
	}

	// 시나리오 검사용 누적 횟수 (ent.windups, ent.slams, ent.hits)
	namespace EntStats
	{
		static int Windups = 0;
		static int Slams = 0;
		static int Hits = 0;
	}

	static Vector2 EntGetPlayerCenter(Skul* skul)
	{
		Collider* collider = skul->GetComponent<Collider>();
		if (collider == nullptr)
			return skul->GetComponent<Transform>()->GetPos();

		return collider->GetPos() + collider->GetSize() / 2.0f;
	}

	Monster::Monster()
		: mAnimator(nullptr)
		, mState(eMonsterState::Idle)
		, mRigidbody(nullptr)
		, mDirection(eDirection::Right)
		, mAttackPhase(eAttackPhase::Windup)
		, mWarning(nullptr)
		, mWindupSound(nullptr)
		, mSlamSound(nullptr)
		, mDeadSound(nullptr)
		, mHP(EntTuning::MaxHP)
		, mTime(0.0f)
		, mDelay(0.0f)
		, mCooldown(0.0f)
		, mHomeX(0.0f)
		, mAnchorX(0.0f)
		, mbAggro(false)
	{
	}
	Monster::~Monster()
	{
	}
	void Monster::Initialize()
	{
		//Goopy Le Grande\Phase 1\Jump
		Transform* tr = GetComponent<Transform>();

		tr->SetScale(Vector2(2.0f, 2.0f));

		mAnimator = AddComponent<Animator>();

		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\LeftIdle", Vector2(-14.0f,-8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\RightIdle", Vector2(-14.0f,-8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\LeftMove", Vector2(-14.0f,-8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\RightMove", Vector2(-14.0f,-8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\LeftHit", Vector2(-14.0f,-8.0f), 0.1f);
		mAnimator->CreateAnimations(L"..\\Resources\\Monster\\Ent\\RightHit", Vector2(-14.0f,-8.0f), 0.1f);


		Collider* collider = AddComponent<Collider>();
		collider->SetCenter(Vector2(-40.0f, -45.0f));
		mRigidbody = AddComponent<Rigidbody>();

		mRigidbody->SetMass(1.0f);
		mRigidbody->SetGround(false);

		mHP = EntTuning::MaxHP;

		// 공격 프레임이 없어서 피격 프레임 두 장을 한 장짜리 자세로 쓴다.
		// 01(팔을 비틀어 든 자세) = 주먹 들기(예고), 02(몸을 숙인 자세) = 내려친 뒤
		createPoseAnimation(L"EntLeftWindup", L"..\\Resources\\Monster\\Ent\\LeftHit\\LeftHit01_0.bmp");
		createPoseAnimation(L"EntRightWindup", L"..\\Resources\\Monster\\Ent\\RightHit\\RightHit01_0.bmp");
		createPoseAnimation(L"EntLeftSlam", L"..\\Resources\\Monster\\Ent\\LeftHit\\LeftHit02_0.bmp");
		createPoseAnimation(L"EntRightSlam", L"..\\Resources\\Monster\\Ent\\RightHit\\RightHit02_0.bmp");

		// 보스(장로 엔트) 사운드 중에서 골랐다: 주먹 들기 / 짧은 충격음 / 쓰러짐
		mWindupSound = Resources::Load<Sound>(L"Ent_Windup", L"..\\Resources\\Sound\\Boss\\ElderEnt_Up 2.wav");
		mSlamSound = Resources::Load<Sound>(L"Ent_SlamImpact", L"..\\Resources\\Sound\\Boss\\ElderEnt_Impact (Legacy).wav");
		mDeadSound = Resources::Load<Sound>(L"Ent_Down", L"..\\Resources\\Sound\\Boss\\ElderEnt_Down.wav");

		mHomeX = tr->GetPos().x;

		//mAnimator->GetStartEvent(L"LeftAttackA") = std::bind(&Monster::StartAttack, this);
		mAnimator->GetCompleteEvent(L"EntLeftHit") = std::bind(&Monster::CompleteHit, this);
		mAnimator->GetCompleteEvent(L"EntRightHit") = std::bind(&Monster::CompleteHit, this);

		mAnimator->GetEndEvent(L"EntLeftHit") = std::bind(&Monster::EndHit, this);
		mAnimator->GetEndEvent(L"EntRightHit") = std::bind(&Monster::EndHit, this);


		mAnimator->Play(L"EntRightIdle", true);
		mState = eMonsterState::Idle;

		// 시나리오 대본이 시작할 때 이름을 확인하므로 미리 내보낸다. 값은 Update 에서 갱신
		Scenario::Publish("ent.hpsum", 0.0);
		Scenario::Publish("ent.nearest", 99999.0);
		Scenario::Publish("ent.windups", 0.0);
		Scenario::Publish("ent.slams", 0.0);
		Scenario::Publish("ent.hits", 0.0);

		GameObject::Initialize();


	}
	void Monster::Update()
	{
		if (mCooldown > 0.0f)
			mCooldown -= (float)Time::DeltaTime();

		switch (mState)
		{
		case ya::Monster::eMonsterState::Idle:
			Idle();
			break;
		case ya::Monster::eMonsterState::Move:
			Move();
			break;
		case ya::Monster::eMonsterState::Attack:
			Attack();
			break;
		case ya::Monster::eMonsterState::LangeAttack:
			LangeAttack();
			break;
		case ya::Monster::eMonsterState::Hit:
			Hit();
			break;
		case ya::Monster::eMonsterState::Dead:
			Dead();
			break;
		default:
			break;
		}

		GameObject::Update();
		publishStats();
	}
	void Monster::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void Monster::Release()
	{
		GameObject::Release();
	}

	void Monster::SetMonsterState(eMonsterState state)
	{
		// 피격은 들어올 때마다 그 자리에서 피해를 적용한다.
		// (예전에는 Hit 상태에서 0.3초 뒤에 HP를 깎았다. 그 사이 3초 배회 타이머가 상태를 덮어쓰면
		//  피해가 사라졌고, Hit 중에 또 맞으면 무시됐다)
		if (state == eMonsterState::Hit)
		{
			takeHit(1);
			return;
		}

		if (mState != state)
		{
			mState = state;
		}
	}



	void Monster::Idle()
	{
		think();
	}
	void Monster::Move()
	{
		think();
		if (mState != eMonsterState::Move)
			return;

		//이동부

		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();

		float speed = mbAggro ? EntTuning::ChaseSpeed : EntTuning::WanderSpeed;
		float step = speed * (float)Time::DeltaTime();
		pos.x += (mDirection == eDirection::Left) ? -step : step;

		// 맵 끝: 배회 중이면 돌아서고, 쫓는 중이면(플레이어가 맵 밖) 그 자리에 선다
		if (clampToMap(pos))
		{
			if (mbAggro)
				changeMove(eMonsterState::Idle, mDirection);
			else
				changeMove(eMonsterState::Move, (mDirection == eDirection::Left) ? eDirection::Right : eDirection::Left);
		}

		tr->SetPos(pos);

	}
	void Monster::Attack()
	{
		mDelay += (float)Time::DeltaTime();

		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();

		if (mAttackPhase == eAttackPhase::Windup)
		{
			// 주먹을 든 채 잠깐 멈췄다가 좌우로 떤다 (1/30초마다 방향을 바꿈)
			float shake = 0.0f;
			if (mDelay >= EntTuning::WindupShakeStart)
				shake = (((int)(mDelay * 30.0f)) % 2 == 0) ? EntTuning::WindupShakeAmount : -EntTuning::WindupShakeAmount;

			pos.x = mAnchorX + shake;
			tr->SetPos(pos);

			if (mDelay >= EntTuning::WindupTime)
				slam();
			return;
		}

		// Recover: 내려친 자세로 멈춰 있다가 다시 판단한다
		if (mDelay >= EntTuning::RecoverTime)
		{
			mDelay = 0.0f;
			mState = eMonsterState::Idle;
			playAnimation(L"Idle", true);
		}
	}
	void Monster::LangeAttack()
	{

	}
	void Monster::Hit()
	{
		// 피해는 takeHit 에서 이미 적용했다. 여기서는 경직과 넉백만
		float deltaTime = (float)Time::DeltaTime();
		mDelay += deltaTime;

		// 때린 쪽(바라보는 쪽)의 반대로 밀려난다
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();
		float push = EntTuning::KnockbackSpeed * deltaTime;
		pos.x += (mDirection == eDirection::Left) ? push : -push;
		clampToMap(pos);
		tr->SetPos(pos);

		if (mDelay >= EntTuning::HitStunTime)
		{
			mDelay = 0.0f;
			mState = eMonsterState::Idle;
			playAnimation(L"Idle", true);
		}
	}
	void Monster::Dead()
	{
		// 마지막 피격 자세로 잠깐 멈췄다가 폭발과 먼지를 남기고 사라진다
		mDelay += (float)Time::DeltaTime();
		if (mDelay < EntTuning::DyingTime)
			return;

		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();
		MonsterEffect::Spawn(MonsterEffect::eType::EntDeathBurst, Vector2(getBodyCenterX(), pos.y + EntTuning::BodyCenterY));
		MonsterEffect::Spawn(MonsterEffect::eType::EntSlamSmoke, Vector2(getBodyCenterX(), pos.y + EntTuning::GroundY));
		mDeadSound->Play(false);

		killWarning();
		SetState(eState::Death);
	}

	void Monster::think()
	{
		Skul* player = findPlayer();

		float dx = 0.0f;
		float dy = 0.0f;
		if (player != nullptr)
		{
			Vector2 target = EntGetPlayerCenter(player);
			Vector2 pos = GetComponent<Transform>()->GetPos();
			dx = target.x - getBodyCenterX();
			dy = target.y - (pos.y + EntTuning::BodyCenterY);
		}

		bool wasAggro = mbAggro;
		if (player == nullptr || PlayerStats::IsDead())
			mbAggro = false;
		else if (!mbAggro)
			mbAggro = fabs(dx) <= EntTuning::DetectRangeX && fabs(dy) <= EntTuning::DetectRangeY;
		else if (fabs(dx) > EntTuning::LoseRangeX || fabs(dy) > EntTuning::LoseRangeY)
			mbAggro = false;

		if (!mbAggro)
		{
			// 놓쳤으면 그 자리에 섰다가 다시 배회한다
			if (wasAggro)
			{
				mTime = 0.0f;
				changeMove(eMonsterState::Idle, mDirection);
			}
			wander();
			return;
		}

		eDirection facing = mDirection;
		if (dx < -EntTuning::FaceDeadZone)
			facing = eDirection::Left;
		else if (dx > EntTuning::FaceDeadZone)
			facing = eDirection::Right;

		// 주먹이 닿는 거리면 멈춘다. 쿨다운이 끝났고 플레이어가 같은 높이면 내려친다
		if (fabs(dx) <= EntTuning::AttackRangeX)
		{
			if (fabs(dy) <= EntTuning::AttackRangeY && mCooldown <= 0.0f)
			{
				mDirection = facing;
				startWindup();
				return;
			}

			changeMove(eMonsterState::Idle, facing);
			return;
		}

		changeMove(eMonsterState::Move, facing);
	}

	void Monster::wander()
	{
		mTime += (float)Time::DeltaTime();

		float fromHome = GetComponent<Transform>()->GetPos().x - mHomeX;

		if (mTime > EntTuning::WanderInterval)
		{
			mTime = 0.0f;

			UINT rstate = rand();
			UINT rDir = rand();
			rstate %= 2;
			rDir %= 2;

			eDirection direction = (eDirection)rDir;

			// 배치된 자리에서 너무 멀어졌으면 그쪽으로 돌아간다
			if (fromHome > EntTuning::WanderLeash)
				direction = eDirection::Left;
			else if (fromHome < -EntTuning::WanderLeash)
				direction = eDirection::Right;

			changeMove((eMonsterState)rstate, direction);
			return;
		}

		// 걷다가 배회 범위 끝에 닿으면 멈춘다 (돌아오는 방향은 계속 걷는다)
		if (mState == eMonsterState::Move
			&& ((mDirection == eDirection::Right && fromHome > EntTuning::WanderLeash)
				|| (mDirection == eDirection::Left && fromHome < -EntTuning::WanderLeash)))
		{
			changeMove(eMonsterState::Idle, mDirection);
		}
	}

	void Monster::changeMove(eMonsterState state, eDirection direction)
	{
		if (mState == state && mDirection == direction)
			return;

		mState = state;
		mDirection = direction;
		playAnimation((state == eMonsterState::Move) ? L"Move" : L"Idle", true);
	}

	void Monster::playAnimation(const std::wstring& action, bool loop)
	{
		// 애니메이션 이름: Ent + Left/Right + 동작 (예: EntLeftIdle)
		std::wstring name = (mDirection == eDirection::Left) ? L"EntLeft" : L"EntRight";
		name += action;
		mAnimator->Play(name, loop);
	}

	void Monster::takeHit(int damage)
	{
		// 죽는 중에는 더 맞지 않는다
		if (mState == eMonsterState::Dead)
			return;

		mHP -= damage;
		EntStats::Hits++;

		// 예고나 내려친 뒤 자세는 맞으면 끊긴다 (일반 엔트는 슈퍼아머가 없다)
		cancelAttack();
		mbAggro = true;

		// 때린 쪽을 바라본다. 넉백은 Hit 에서 반대쪽으로
		Skul* player = findPlayer();
		if (player != nullptr)
			mDirection = (EntGetPlayerCenter(player).x < getBodyCenterX()) ? eDirection::Left : eDirection::Right;

		mDelay = 0.0f;
		if (mHP <= 0)
		{
			mHP = 0;
			mState = eMonsterState::Dead;
		}
		else
		{
			mState = eMonsterState::Hit;
		}

		// 연달아 맞아도 피격 애니메이션을 처음부터 다시 재생한다
		playAnimation(L"Hit", false);
	}

	void Monster::startWindup()
	{
		mState = eMonsterState::Attack;
		mAttackPhase = eAttackPhase::Windup;
		mDelay = 0.0f;
		mAnchorX = GetComponent<Transform>()->GetPos().x;
		playAnimation(L"Windup", false);

		// 주먹이 떨어질 바닥에 붉은 경고. 내려치거나 끊기면 지운다
		killWarning();
		mWarning = MonsterEffect::Spawn(MonsterEffect::eType::EntWarning, getFistPoint());
		mWindupSound->Play(false);
		EntStats::Windups++;
	}

	void Monster::slam()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();
		pos.x = mAnchorX;
		tr->SetPos(pos);
		killWarning();

		// 판정은 주먹이 닿는 바닥 위로 잠깐만 있다. 피해 방향은 엔트 몸 쪽에서
		Vector2 fist = getFistPoint();
		Vector2 body(getBodyCenterX(), pos.y + EntTuning::BodyCenterY);
		EntSlamHitbox::Spawn(fist, Vector2(EntTuning::SlamWidth, EntTuning::SlamHeight)
			, EntTuning::SlamDamage, body, EntTuning::SlamActiveTime);
		MonsterEffect::Spawn(MonsterEffect::eType::EntSlamSmoke, fist);
		Camera::Shake(4.0f, 0.15f);
		mSlamSound->Play(false);
		EntStats::Slams++;

		mAttackPhase = eAttackPhase::Recover;
		mDelay = 0.0f;
		mCooldown = EntTuning::SlamCooldown;
		playAnimation(L"Slam", false);
	}

	void Monster::cancelAttack()
	{
		if (mState != eMonsterState::Attack)
			return;

		// 떨던 위치를 되돌린다
		if (mAttackPhase == eAttackPhase::Windup)
		{
			Transform* tr = GetComponent<Transform>();
			Vector2 pos = tr->GetPos();
			pos.x = mAnchorX;
			tr->SetPos(pos);
		}
		killWarning();
	}

	void Monster::killWarning()
	{
		if (mWarning == nullptr)
			return;

		mWarning->SetState(eState::Death);
		mWarning = nullptr;
	}

	bool Monster::clampToMap(Vector2& pos)
	{
		// 그림이 스테이지 밖으로 나가지 않게 Transform x 를 제한한다
		float minX = EntTuning::MapLeft - EntTuning::BodyLeft;
		float maxX = EntTuning::MapRight - EntTuning::BodyRight;

		if (pos.x < minX)
		{
			pos.x = minX;
			return true;
		}
		if (pos.x > maxX)
		{
			pos.x = maxX;
			return true;
		}
		return false;
	}

	void Monster::createPoseAnimation(const std::wstring& name, const std::wstring& path)
	{
		// 폴더 애니메이션과 같은 캐시 키(전체 경로)로 읽어서 피격 애니메이션과 이미지를 같이 쓴다
		Image* image = Resources::Load<Image>(path, path);
		if (image == nullptr)
			return;

		// 피격 애니메이션(시트 56x67, 오프셋 -14,-8)과 같은 자리에 그려지게 맞춘다.
		// Animation::Render 는 왼쪽 위를 pos + offset - (w/2, h) 에 두고 배율 2로 그리므로
		// 가운데 x = offset.x + w/2, 아래 끝 y = offset.y + h. 이것을 +14, +59 에 맞춘다
		float width = (float)image->GetWidth();
		float height = (float)image->GetHeight();
		Vector2 offset(14.0f - width / 2.0f, 59.0f - height);
		mAnimator->CreateAnimation(name, image, Vector2::Zero, 1, 1, 1, 1, offset, 0.1f);
	}

	void Monster::publishStats()
	{
		Scene* scene = SceneManager::GetActiveScene();
		Skul* player = findPlayer();
		float playerX = (player != nullptr) ? EntGetPlayerCenter(player).x : 0.0f;

		// 보스 손도 Monster 레이어에 있으므로 엔트(Monster)만 센다
		double hpSum = 0.0;
		double nearest = 99999.0;
		for (GameObject* obj : scene->GetGameObjects(eLayerType::Monster))
		{
			Monster* ent = dynamic_cast<Monster*>(obj);
			if (ent == nullptr
				|| ent->GetState() != eState::Active
				|| ent->mState == eMonsterState::Dead)
				continue;

			hpSum += ent->mHP;
			if (player != nullptr)
			{
				double distance = fabs(ent->getBodyCenterX() - playerX);
				if (distance < nearest)
					nearest = distance;
			}
		}

		Scenario::Publish("ent.hpsum", hpSum);
		Scenario::Publish("ent.nearest", nearest);
		Scenario::Publish("ent.windups", EntStats::Windups);
		Scenario::Publish("ent.slams", EntStats::Slams);
		Scenario::Publish("ent.hits", EntStats::Hits);
	}

	Skul* Monster::findPlayer()
	{
		PlayeScene* playScene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
		if (playScene == nullptr)
			return nullptr;

		Skul* skul = playScene->GetSkul();
		if (skul == nullptr || skul->GetState() != eState::Active)
			return nullptr;

		return skul;
	}

	float Monster::getBodyCenterX()
	{
		return GetComponent<Transform>()->GetPos().x + EntTuning::BodyCenterX;
	}

	Vector2 Monster::getFistPoint()
	{
		Vector2 pos = GetComponent<Transform>()->GetPos();
		float reach = (mDirection == eDirection::Left) ? -EntTuning::FistReach : EntTuning::FistReach;
		return Vector2(getBodyCenterX() + reach, pos.y + EntTuning::GroundY);
	}

	void Monster::CompleteIdle()
	{
	}

	void Monster::CompleteHit()
	{



	}

	void Monster::EndIdle()
	{
	}
	void Monster::EndHit()
	{
	}
}