#include "yaChapter1_Boss.h"
#include "yaTransform.h"
#include "yaCamera.h"
#include "yaObject.h"
#include "yaBoss_Body.h"
#include "yaBoss_Hand.h"
#include "yaBoss_RightHand.h"
#include "yaBoss_Head.h"
#include "yaBoss_Chin.h"
#include "yaBossBullet.h"
#include "yaBossEffect.h"
#include "yaBossHpBar.h"
#include "yaTime.h"
#include "yaSkul.h"			// yaPlayeScene.h가 Skul을 쓰므로 먼저 (unity 빌드에서 순서가 섞인다)
#include "yaPlayeScene.h"
#include "yaPlatform.h"
#include "yaSound.h"
#include "yaResources.h"
#include "yaScenario.h"
#include "yaInput.h"

namespace ya
{
	// ---- 페이즈별 패턴 수치 (원작: 2페이즈가 더 빠르고 횟수가 늘어난다) ----
	static const Chapter1_Boss::Timing BossPhase1Timing =
	{
		1.6f,		// idle
		1.0f,		// punchRaise
		2600.0f,	// punchSlamSpeed
		0.9f,		// punchStay
		2,			// punchCount
		1.0f,		// sweepReady (원작: 기울어지고 약 1초 뒤 쓸기)
		2600.0f,	// sweepSpeed
		2,			// sweepCount
		4.5f,		// groggy
	};
	static const Chapter1_Boss::Timing BossPhase2Timing =
	{
		1.0f,		// idle
		0.6f,		// punchRaise
		3400.0f,	// punchSlamSpeed
		0.5f,		// punchStay
		4,			// punchCount (2+2)
		0.7f,		// sweepReady
		3200.0f,	// sweepSpeed
		4,			// sweepCount (좌우 2+2)
		4.0f,		// groggy
	};

	// ---- 에너지볼 ----
	static const float BossEnergyCharge = 2.0f;			// 1페이즈 충전 (원작 2초)
	static const float BossEnergyRecover = 0.6f;		// 발사 뒤 다음 충전까지
	static const int BossEnergyVolleys = 3;				// 3회 반복 후 그로기
	static const float BossSmallBallSpeed = 420.0f;
	static const float BossSmallBallScale = 0.9f;
	static const float BossBigBallSpeed = 380.0f;
	static const float BossBigBallScale = 1.6f;
	static const float BossRainCharge = 1.0f;			// 2페이즈 충전 (원작 1초)
	static const int BossRainCount = 12;
	static const float BossRainInterval = 0.18f;
	static const float BossRainSpeed = 330.0f;
	static const float BossRainHoming = 240.0f;			// 약한 유도 (px/초²)
	static const float BossRainScale = 1.2f;

	// ---- 양손 3연타 ----
	static const float BossTripleRaise = 0.9f;			// 들어 올린 채 예고하는 시간
	static const float BossTripleLateExtra = 0.7f;		// 세 번째는 이만큼 늦게 내려친다
	static const float BossTripleSlamSpeed = 3600.0f;
	static const float BossTripleStay = 0.45f;

	// ---- 위치 (월드 좌표, y는 아래로 갈수록 커짐. 지면 충돌체 윗변 1750, 서 있는 플레이어 y 1726) ----
	static const float BossCenterOffsetX = 215.0f;		// 보스 위치 기준 몸 가운데(이마 마석)의 x
	static const float BossHandSpread = 430.0f;			// 몸 가운데에서 손까지
	static const float BossHoverY = 1652.0f;			// 대기 중 손 아래쪽 높이
	static const float BossRaisedY = 1390.0f;			// 주먹을 들어 올린 높이
	static const float BossTripleRaisedY = 1360.0f;
	static const float BossFistFloorY = 1790.0f;		// 주먹·쓸기 손이 바닥에 닿는 높이
	static const float BossFlatY = 1800.0f;				// 손바닥으로 바닥을 짚는 높이
	static const float BossDustY = 1758.0f;				// 흙먼지 이펙트 바닥
	static const float BossArenaLeft = 0.0f;			// 카메라 경계와 같은 맵 범위
	static const float BossArenaRight = 3350.0f;
	static const float BossSweepOutside = 320.0f;		// 쓸기 시작·끝은 맵 밖 (화면에 보이지 않는 곳)
	static const float BossCollapseY = 300.0f;			// 그로기·사망 때 몸과 머리가 내려앉는 거리

	static float BossClamp(float value, float low, float high)
	{
		if (value < low)
			return low;
		if (value > high)
			return high;
		return value;
	}

	Chapter1_Boss::Chapter1_Boss()
		: mBody(nullptr)
		, mHead(nullptr)
		, mChin(nullptr)
		, mLeftHand(nullptr)
		, mRightHand(nullptr)
		, mBullets{}
		, mEffects{}
		, mTelegraph(nullptr)
		, mHpBar(nullptr)
		, mPlatform{}
		, mState(eBossState::Intro)
		, mLastPattern(eBossState::Idle)
		, mStateTime(0.0f)
		, mStepTime(0.0f)
		, mStep(0)
		, mbStepEnter(true)
		, mbStepChanged(false)
		, mCount(0)
		, mSweepRight(false)
		, mSlamRight(false)
		, mSlamCenterX(0.0f)
		, mCenterX(0.0f)
		, mBigBall(nullptr)
		, mHP(MaxHP)
		, mPhase(1)
		, mVolleys(0)
		, mbGroggy(false)
		, mbBarVisible(false)
	{
	}
	Chapter1_Boss::~Chapter1_Boss()
	{
	}
	void Chapter1_Boss::Initialize()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 bosspos = tr->GetPos();
		mCenterX = bosspos.x + BossCenterOffsetX;

		// 부품끼리는 레이어 안 순서(index)가 아니라 포인터로 연결한다
		mBody = object::Instantiate<Boss_Body>(bosspos, eLayerType::Boss);
		mHead = object::Instantiate<Boss_Head>(bosspos, eLayerType::Boss);
		mChin = object::Instantiate<Boss_Chin>(bosspos, eLayerType::Boss);
		// 예고는 Boss 레이어 마지막: 몸 위, 손·플레이어 아래에 그려진다
		mTelegraph = object::Instantiate<BossTelegraph>(bosspos, eLayerType::Boss);
		mLeftHand = object::Instantiate<Boss_Hand>(hoverHome(false), eLayerType::Monster);
		mRightHand = object::Instantiate<Boss_RightHand>(hoverHome(true), eLayerType::Monster);

		mBody->SetBoss(this);
		mHead->SetBoss(this);
		mChin->SetHead(mHead);
		mLeftHand->SetBoss(this);
		mRightHand->SetBoss(this);

		// 1페이즈 뿌리 발판. 2페이즈가 되면 없앤다
		mPlatform[0] = object::Instantiate<Platform>(Vector2(bosspos.x - 50.0f, bosspos.y + 200.0f), eLayerType::Ground);
		mPlatform[1] = object::Instantiate<Platform>(Vector2(bosspos.x + 350.0f, bosspos.y + 120.0f), eLayerType::Ground);

		for (int i = 0; i < BulletCount; i++)
			mBullets[i] = object::Instantiate<BossBullet>(bosspos, eLayerType::EnemyBullet);
		for (int i = 0; i < EffectCount; i++)
			mEffects[i] = object::Instantiate<BossEffect>(bosspos, eLayerType::Effect);

		mHpBar = object::Instantiate<BossHpBar>(eLayerType::UI);
		mHpBar->SetBoss(this);

		mAppearance = Resources::Load<Sound>(L"ElderEnt_Appearance", L"..\\Resources\\Sound\\Boss\\ElderEnt_Appearance.wav");
		mRoar = Resources::Load<Sound>(L"ElderEnt_Roar", L"..\\Resources\\Sound\\Boss\\ElderEnt_Roar.wav");
		mRoarShort = Resources::Load<Sound>(L"ElderEnt_Roar_Short", L"..\\Resources\\Sound\\Boss\\ElderEnt_Roar_Short.wav");
		mFistSlam = Resources::Load<Sound>(L"ElderEnt_FistSlam", L"..\\Resources\\Sound\\Boss\\ElderEnt_FistSlam.wav");
		mFistSlamRecovery = Resources::Load<Sound>(L"ElderEnt_FistSlam_Recovery", L"..\\Resources\\Sound\\Boss\\ElderEnt_FistSlam_Recovery.wav");
		mSweepingReady = Resources::Load<Sound>(L"ElderEnt_Sweeping_Ready", L"..\\Resources\\Sound\\Boss\\ElderEnt_Sweeping_Ready.wav");
		mSweeping = Resources::Load<Sound>(L"ElderEnt_Sweeping", L"..\\Resources\\Sound\\Boss\\ElderEnt_Sweeping.wav");
		mEnergyReady = Resources::Load<Sound>(L"ElderEnt_EnergyBomb_Ready", L"..\\Resources\\Sound\\Boss\\ElderEnt_EnergyBomb_Ready.wav");
		mEnergyFire = Resources::Load<Sound>(L"ElderEnt_EnergyBomb_Fire", L"..\\Resources\\Sound\\Boss\\ElderEnt_EnergyBomb_Fire.wav");
		mGroggyImpact = Resources::Load<Sound>(L"ElderEnt_Groggy_Impact", L"..\\Resources\\Sound\\Boss\\ElderEnt_Groggy_Impact.wav");
		mGroggyRecovery = Resources::Load<Sound>(L"ElderEnt_Groggy_Recovery", L"..\\Resources\\Sound\\Boss\\ElderEnt_Groggy_Recovery.wav");
		mDown = Resources::Load<Sound>(L"ElderEnt_Down", L"..\\Resources\\Sound\\Boss\\ElderEnt_Down.wav");
		mUp = Resources::Load<Sound>(L"ElderEnt_Up_1", L"..\\Resources\\Sound\\Boss\\ElderEnt_Up 1.wav");
		mBothFistRoar = Resources::Load<Sound>(L"ElderEnt_Roar_BothFistPowerSlam", L"..\\Resources\\Sound\\Boss\\ElderEnt_Roar_BothFistPowerSlam.wav");
		mDeadImpact = Resources::Load<Sound>(L"ElderEnt_Dead_Impact", L"..\\Resources\\Sound\\Boss\\ElderEnt_Dead_Impact.wav");
		mDeadRoar = Resources::Load<Sound>(L"ElderEnt_Dead_Roar", L"..\\Resources\\Sound\\Boss\\ElderEnt_Dead_Roar.wav");

		// 등장 연출 시작 자세: 몸과 머리는 땅속에, 두 손은 들려 있다.
		// 보스는 Boss 씬이 활성일 때만 Update되므로 연출은 씬에 들어간 순간부터 흐른다.
		mBody->SnapOffset(Vector2(0.0f, BossCollapseY));
		mHead->SnapOffset(Vector2(0.0f, BossCollapseY));
		mLeftHand->Teleport(raisedPos(false));
		mRightHand->Teleport(raisedPos(true));

		mHP = MaxHP;
		mPhase = 1;
		enterState(eBossState::Intro);
		// 시나리오는 시작할 때 변수 이름을 검사하므로 처음 값을 바로 내보낸다
		publish();

		GameObject::Initialize();
	}
	void Chapter1_Boss::Update()
	{
		mbStepChanged = false;

		switch (mState)
		{
		case eBossState::Intro:
			intro();
			break;
		case eBossState::Idle:
			idle();
			break;
		case eBossState::Punch:
			punch();
			break;
		case eBossState::Sweep:
			sweep();
			break;
		case eBossState::EnergyBall:
			energyBall();
			break;
		case eBossState::Groggy:
			groggy();
			break;
		case eBossState::PhaseChange:
			phaseChange();
			break;
		case eBossState::EnergyRain:
			energyRain();
			break;
		case eBossState::TripleSlam:
			tripleSlam();
			break;
		case eBossState::Dead:
			dead();
			break;
		default:
			break;
		}

		// 이번 프레임에 단계가 바뀌었으면(enterState/setStep) 시간은 0인 채로 두고,
		// 다음 프레임에 상태 함수가 mbStepEnter = true로 그 단계의 시작 처리를 한다.
		// 피격 콜백(충돌 처리 중)에서 단계가 바뀌어도 같은 방식으로 다음 프레임에 시작한다.
		if (!mbStepChanged)
		{
			mbStepEnter = false;
			float dt = (float)Time::DeltaTime();
			mStateTime += dt;
			mStepTime += dt;
		}

#ifdef _DEBUG
		// 개발용 (Debug 빌드만): P 키로 다음 페이즈로 건너뛴다. 1페이즈 -> 2페이즈 전환, 2페이즈 -> 사망.
		// 실제 공격으로 2페이즈까지 가면 약 1500프레임(헤드리스 실행 90초 이상)이 걸려서,
		// 2페이즈·사망 연출 시나리오(Scenarios/g4_phase2_death.txt)는 이 키로 짧게 확인한다.
		// 피격 콜백과 같은 자리(상태 함수가 끝난 뒤)에서 바꿔야 새 상태의 시작 처리가 다음 프레임에 한 번만 돈다.
		if (Input::GetKeyDown(eKeyCode::P))
			debugSkipPhase();
#endif

		publish();
		GameObject::Update();
	}
	void Chapter1_Boss::Render(HDC hdc)
	{
		GameObject::Render(hdc);
	}
	void Chapter1_Boss::Release()
	{
		GameObject::Release();
	}

	// ---------------------------------------------------------------------------------------------
	// 피격

	bool Chapter1_Boss::OnHeadHit()
	{
		return applyDamage(1);
	}

	void Chapter1_Boss::OnPartAttacked(ePart part)
	{
		// 그로기가 아니면 머리만 맞는다 (머리는 SkulAttack이 직접 Boss_Head::Hit을 부른다)
		if (!mbGroggy || part == ePart::Head)
			return;

		// SkulAttack이 머리에 적용하는 것과 같은 조건: 지상·점프 공격 중이고, 이번 공격이 아직 아무것도 맞히지 않음
		PlayeScene* scene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
		Skul* skul = (scene != nullptr) ? scene->GetSkul() : nullptr;
		if (skul == nullptr)
			return;

		Skul::eSkulState state = skul->GetSkulState();
		if (state != Skul::eSkulState::Attack && state != Skul::eSkulState::JumpAttack)
			return;
		if (!skul->GetIsAttack())
			return;

		if (applyDamage(1))
			skul->SetIsAttack(false);
	}

	void Chapter1_Boss::debugSkipPhase()
	{
		if (mState == eBossState::Intro || mState == eBossState::PhaseChange || mState == eBossState::Dead)
			return;

		if (mPhase == 1)
			applyDamage(mHP - Phase2HP);
		else
			applyDamage(mHP);
	}

	bool Chapter1_Boss::applyDamage(int amount)
	{
		// 등장·페이즈 전환·사망 연출 중에는 맞지 않는다
		if (mState == eBossState::Intro || mState == eBossState::PhaseChange || mState == eBossState::Dead)
			return false;

		mHP -= amount;
		if (mPhase == 1 && mHP <= Phase2HP)
		{
			// 1페이즈에서 남은 피해는 넘기지 않는다
			mHP = Phase2HP;
			enterState(eBossState::PhaseChange);
		}
		else if (mHP <= 0)
		{
			mHP = 0;
			enterState(eBossState::Dead);
		}
		publish();
		return true;
	}

	// ---------------------------------------------------------------------------------------------
	// 상태 공통

	void Chapter1_Boss::enterState(eBossState state)
	{
		mState = state;
		mStateTime = 0.0f;
		mCount = 0;
		setStep(0);
	}

	void Chapter1_Boss::setStep(int step)
	{
		mStep = step;
		mStepTime = 0.0f;
		mbStepEnter = true;
		mbStepChanged = true;
	}

	const Chapter1_Boss::Timing& Chapter1_Boss::timing()
	{
		return (mPhase >= 2) ? BossPhase2Timing : BossPhase1Timing;
	}

	void Chapter1_Boss::publish()
	{
		Scenario::Publish("boss.hp", mHP);
		Scenario::Publish("boss.phase", mPhase);
		Scenario::Publish("boss.state", (double)(int)mState);
		Scenario::Publish("boss.groggy", mbGroggy ? 1.0 : 0.0);
		Scenario::Publish("boss.volleys", mVolleys);
	}

	Chapter1_Boss::eBossState Chapter1_Boss::pickPattern()
	{
		// 같은 패턴이 연달아 나오지 않게 한다
		if (mPhase >= 2)
		{
			static const eBossState phase2[4] = { eBossState::Punch, eBossState::Sweep, eBossState::EnergyRain, eBossState::TripleSlam };
			int index = rand() % 4;
			if (phase2[index] == mLastPattern)
				index = (index + 1) % 4;
			return phase2[index];
		}

		static const eBossState phase1[3] = { eBossState::Punch, eBossState::Sweep, eBossState::EnergyBall };
		int index = rand() % 3;
		if (phase1[index] == mLastPattern)
			index = (index + 1) % 3;
		return phase1[index];
	}

	// ---------------------------------------------------------------------------------------------
	// 등장: 땅속에서 올라오며 두 손으로 땅을 움켜쥐고 포효한다. 끝나면 보스 바가 나온다

	void Chapter1_Boss::intro()
	{
		switch (mStep)
		{
		case 0:
			if (mbStepEnter)
			{
				mBody->SetOffset(Vector2::Zero, 430.0f);
				mHead->SetOffset(Vector2::Zero, 430.0f);
				mLeftHand->SetTremble(true);
				mRightHand->SetTremble(true);
			}
			if (mStepTime >= 0.7f)
			{
				mLeftHand->SetTremble(false);
				mRightHand->SetTremble(false);
				mLeftHand->SetPose(Boss_Hand::ePose::Flat);
				mRightHand->SetPose(Boss_Hand::ePose::Flat);
				mLeftHand->MoveTo(restPos(false), 2400.0f);
				mRightHand->MoveTo(restPos(true), 2400.0f);
				setStep(1);
			}
			break;
		case 1:
			if (mLeftHand->IsArrived() && mRightHand->IsArrived())
			{
				playEffect((int)BossEffect::eType::AppearanceImpact, Vector2(restPos(false).x, BossDustY));
				playEffect((int)BossEffect::eType::AppearanceImpact, Vector2(restPos(true).x, BossDustY));
				Camera::Shake(16.0f, 0.7f);
				mAppearance->Play(false);
				setStep(2);
			}
			break;
		case 2:
			if (mStepTime >= 0.5f)
			{
				mRoar->Play(false);
				Camera::Shake(6.0f, 1.2f);
				mChin->SetOpen(true);
				setStep(3);
			}
			break;
		case 3:
			if (mStepTime >= 1.2f)
			{
				mChin->SetOpen(false);
				handsHome(900.0f);
				mbBarVisible = true;
				enterState(eBossState::Idle);
			}
			break;
		default:
			break;
		}
	}

	void Chapter1_Boss::idle()
	{
		if (mbStepEnter)
		{
			setPartsOffset(Vector2::Zero, 600.0f);
			mHead->SetBob(true);
			mChin->SetOpen(false);
		}

		if (mStateTime >= timing().idle)
		{
			mLastPattern = pickPattern();
			enterState(mLastPattern);
		}
	}

	// ---------------------------------------------------------------------------------------------
	// 주먹: 두 주먹을 들고 떨다가(예고) 플레이어와 가까운 쪽 주먹으로 플레이어 위치를 내려친다.
	// 1페이즈 2회, 2페이즈 2+2회(두 번째 묶음 앞에 잠깐 쉼)

	void Chapter1_Boss::punch()
	{
		const Timing& t = timing();
		switch (mStep)
		{
		case 0: // 들어 올리고 떨기
		{
			if (mbStepEnter)
			{
				for (bool right : { false, true })
				{
					Boss_Hand* h = hand(right);
					h->SetPose(Boss_Hand::ePose::Fist);
					h->MoveTo(raisedPos(right), 1400.0f);
					h->SetTremble(true);
				}
				if (mCount == 0)
					mRoarShort->Play(false);
			}

			float raise = t.punchRaise;
			if (mCount > 0)
				raise = (mPhase >= 2 && mCount == 2) ? t.punchRaise + 0.4f : t.punchRaise * 0.7f;

			if (mStepTime >= raise && mLeftHand->IsArrived() && mRightHand->IsArrived())
			{
				Vector2 player = playerPos();
				mSlamRight = fabsf(player.x - hoverHome(true).x) < fabsf(player.x - hoverHome(false).x);

				Boss_Hand* h = hand(mSlamRight);
				h->SetTremble(false);
				h->SetAttack(FistDamage);
				h->MoveTo(Vector2(BossClamp(player.x, BossArenaLeft + 150.0f, BossArenaRight - 150.0f), BossFistFloorY), t.punchSlamSpeed);
				setStep(1);
			}
			break;
		}
		case 1: // 내려치는 중
			if (hand(mSlamRight)->IsArrived())
			{
				mFistSlam->Play(false);
				Camera::Shake(9.0f, 0.35f);
				playEffect((int)BossEffect::eType::AppearanceImpact, Vector2(hand(mSlamRight)->GetPos().x, BossDustY));
				setStep(2);
			}
			break;
		case 2: // 바닥에 머무름. 피해는 닿는 순간(0.12초)만
			if (mStepTime >= 0.12f)
				hand(mSlamRight)->SetAttack(0);
			if (mStepTime >= t.punchStay)
			{
				mCount++;
				mFistSlamRecovery->Play(false);
				hand(mSlamRight)->MoveTo(raisedPos(mSlamRight), 1600.0f);
				setStep(3);
			}
			break;
		case 3: // 들어 올린 자리로 돌아옴
			if (hand(mSlamRight)->IsArrived())
			{
				if (mCount < t.punchCount)
				{
					setStep(0);
				}
				else
				{
					handsHome(900.0f);
					setStep(4);
				}
			}
			break;
		case 4:
			if ((mLeftHand->IsArrived() && mRightHand->IsArrived()) || mStepTime >= 1.0f)
				enterState(eBossState::Idle);
			break;
		default:
			break;
		}
	}

	// ---------------------------------------------------------------------------------------------
	// 쓸기: 한 손을 화면 밖으로 벌리고 그쪽으로 기울어진 뒤(예고) 화면이 흔들리며 반대편까지 바닥을 쓴다.
	// 1페이즈 2회(방향 무작위), 2페이즈 좌우 번갈아 2+2회

	void Chapter1_Boss::sweep()
	{
		const Timing& t = timing();
		switch (mStep)
		{
		case 0: // 준비: 쓰는 손은 맵 밖으로, 다른 손은 바닥을 짚고, 몸은 쓰는 쪽으로 기운다
		{
			if (mbStepEnter)
			{
				if (mPhase >= 2 && mCount > 0)
					mSweepRight = !mSweepRight;
				else
					mSweepRight = (rand() % 2) == 1;

				Boss_Hand* sweeper = hand(mSweepRight);
				Boss_Hand* other = hand(!mSweepRight);
				Vector2 start = sweepOutside(mSweepRight);

				// 지난번 쓸기로 반대편 맵 밖에 있으면 화면에 안 보이는 곳이니 바로 옮긴다
				Vector2 sweeperPos = sweeper->GetPos();
				if (sweeperPos.x < BossArenaLeft || sweeperPos.x > BossArenaRight)
					sweeper->Teleport(start);

				sweeper->SetPose(Boss_Hand::ePose::Sweep);
				sweeper->SetAttack(0);
				sweeper->MoveTo(start, 2400.0f);

				Vector2 otherPos = other->GetPos();
				if (otherPos.x < BossArenaLeft || otherPos.x > BossArenaRight)
					other->Teleport(sweepOutside(!mSweepRight));
				other->SetPose(Boss_Hand::ePose::Flat);
				other->SetAttack(0);
				other->MoveTo(restPos(!mSweepRight), 1800.0f);

				setPartsOffset(Vector2(mSweepRight ? 150.0f : -150.0f, 30.0f), 400.0f);
				mSweepingReady->Play(false);
			}

			if (mStepTime >= t.sweepReady && hand(mSweepRight)->IsArrived())
			{
				float distance = BossArenaRight - BossArenaLeft + BossSweepOutside * 2.0f;
				float duration = distance / t.sweepSpeed;

				Boss_Hand* sweeper = hand(mSweepRight);
				sweeper->SetAttack(SweepDamage);
				sweeper->MoveTo(sweepOutside(!mSweepRight), t.sweepSpeed);
				setPartsOffset(Vector2(mSweepRight ? -150.0f : 150.0f, 30.0f), 300.0f / duration);
				Camera::Shake(10.0f, duration);
				mSweeping->Play(false);
				setStep(1);
			}
			break;
		}
		case 1: // 쓰는 중
			if (hand(mSweepRight)->IsArrived())
			{
				hand(mSweepRight)->SetAttack(0);
				mCount++;
				if (mCount < t.sweepCount)
				{
					setStep(0);
				}
				else
				{
					// 반대편 맵 밖에서 자기 쪽 맵 밖으로 옮긴 뒤 제자리로
					hand(mSweepRight)->Teleport(sweepOutside(mSweepRight));
					handsHome(1800.0f);
					setPartsOffset(Vector2::Zero, 400.0f);
					setStep(2);
				}
			}
			break;
		case 2:
			if ((mLeftHand->IsArrived() && mRightHand->IsArrived()) || mStepTime >= 1.2f)
				enterState(eBossState::Idle);
			break;
		default:
			break;
		}
	}

	// ---------------------------------------------------------------------------------------------
	// 에너지볼(1페이즈): 이마 마석에 2초 충전 -> 조준 큰 구체 + 8방향 작은 구체. 3회 반복 후 그로기

	void Chapter1_Boss::energyBall()
	{
		switch (mStep)
		{
		case 0: // 충전
			if (mbStepEnter)
			{
				if (mCount == 0)
					mVolleys = 0;

				handsHome(900.0f);
				mChin->SetOpen(true);
				mEnergyReady->Play(false);
				mBigBall = acquireBullet();
				if (mBigBall != nullptr)
					mBigBall->Charge(mHead->GetGemPos(), BossBigBallScale);
			}

			if (mStepTime >= BossEnergyCharge)
			{
				Vector2 gem = mHead->GetGemPos();

				// 큰 구체는 쏘는 순간의 플레이어 위치로
				if (mBigBall != nullptr && mBigBall->IsCharging())
				{
					Vector2 target = playerPos();
					target.y -= 20.0f;
					Vector2 aim = target - gem;
					if (aim.Length() < 1.0f)
						aim = Vector2(0.0f, 1.0f);
					mBigBall->Launch(aim.Normalize() * BossBigBallSpeed, BigBallDamage);
				}
				mBigBall = nullptr;

				// 8방향. 대각선도 정규화된 방향이라 속도가 같다 (예전에는 대각선이 약 1.4배 빨랐다)
				for (int i = 0; i < 8; i++)
				{
					BossBullet* bullet = acquireBullet();
					if (bullet == nullptr)
						break;

					float radian = (float)PI * 0.25f * (float)i;
					Vector2 direction = Vector2(cosf(radian), sinf(radian));
					bullet->Charge(gem, BossSmallBallScale);
					bullet->Launch(direction * BossSmallBallSpeed, SmallBallDamage);
				}

				mEnergyFire->Play(false);
				mChin->SetOpen(false);
				mVolleys++;
				mCount++;
				setStep(1);
			}
			break;
		case 1: // 다음 충전까지
			if (mStepTime >= BossEnergyRecover)
			{
				if (mCount < BossEnergyVolleys)
					setStep(0);
				else
					enterState(eBossState::Groggy);
			}
			break;
		default:
			break;
		}
	}

	// ---------------------------------------------------------------------------------------------
	// 그로기: 쓰러져서 몸 전체를 때릴 수 있다

	void Chapter1_Boss::groggy()
	{
		switch (mStep)
		{
		case 0:
			if (mbStepEnter)
			{
				mbGroggy = true;
				setPartsOffset(Vector2(0.0f, BossCollapseY), 900.0f);
				mHead->SetBob(false);
				mChin->SetOpen(false);
				handsRest(1400.0f);
				mDown->Play(false);
				mGroggyImpact->Play(false);
				Camera::Shake(8.0f, 0.4f);
			}
			if (mStepTime >= timing().groggy)
			{
				mbGroggy = false;
				mGroggyRecovery->Play(false);
				mUp->Play(false);
				setPartsOffset(Vector2::Zero, 600.0f);
				mHead->SetBob(true);
				handsHome(900.0f);
				setStep(1);
			}
			break;
		case 1:
			if (mStepTime >= 0.7f)
				enterState(eBossState::Idle);
			break;
		default:
			break;
		}
	}

	// ---------------------------------------------------------------------------------------------
	// 2페이즈 전환: 포효하며 보라색으로 바뀌고 뿌리 발판이 사라진다. 끝나면 새 패턴(양손 3연타)부터

	void Chapter1_Boss::phaseChange()
	{
		switch (mStep)
		{
		case 0:
			if (mbStepEnter)
			{
				mPhase = 2;
				mbGroggy = false;
				stopAllBullets();
				mTelegraph->Hide();
				handsHome(1200.0f);
				mLeftHand->SetTremble(true);
				mRightHand->SetTremble(true);
				setPartsOffset(Vector2::Zero, 600.0f);
				mHead->SetBob(true);
				mChin->SetOpen(true);
				mBody->SetLook(Boss_Body::eLook::Flash);
				mRoar->Play(false);
				Camera::Shake(12.0f, 1.6f);

				// 뿌리 발판 제거. G1의 픽셀 충돌 맵으로 바뀌면 맵을 다시 그리는 연결이 필요하다
				for (Platform*& platform : mPlatform)
				{
					if (platform != nullptr)
					{
						platform->SetState(eState::Death);
						platform = nullptr;
					}
				}
			}
			if (mStepTime >= 0.6f)
			{
				setPartsPhase2(true);
				setStep(1);
			}
			break;
		case 1:
			if (mStepTime >= 1.4f)
			{
				mLeftHand->SetTremble(false);
				mRightHand->SetTremble(false);
				mChin->SetOpen(false);
				mLastPattern = eBossState::TripleSlam;
				enterState(eBossState::TripleSlam);
			}
			break;
		default:
			break;
		}
	}

	// ---------------------------------------------------------------------------------------------
	// 에너지볼 비(2페이즈): 1초 충전 뒤 큰 구체가 화면 곳곳에서 쏟아진다(약한 유도). 끝나면 그로기

	void Chapter1_Boss::energyRain()
	{
		switch (mStep)
		{
		case 0:
			if (mbStepEnter)
			{
				handsHome(900.0f);
				mChin->SetOpen(true);
				mEnergyReady->Play(false);
				mBigBall = acquireBullet();
				if (mBigBall != nullptr)
					mBigBall->Charge(mHead->GetGemPos(), BossBigBallScale);
			}
			if (mStepTime >= BossRainCharge)
			{
				// 모은 구체를 하늘로 쏘아 올린다 (화면 위로 나가면 사라진다)
				if (mBigBall != nullptr && mBigBall->IsCharging())
					mBigBall->Launch(Vector2(0.0f, -1100.0f), 0);
				mBigBall = nullptr;
				mEnergyFire->Play(false);
				mChin->SetOpen(false);
				setStep(1);
			}
			break;
		case 1:
			// 화면 위에서 하나씩 떨어뜨린다. x는 지금 화면 안에서 무작위
			if (mStepTime >= BossRainInterval * mCount)
			{
				Vector2 look = Camera::GetLookPosition();
				float x = look.x - 700.0f + (float)(rand() % 1400);
				x = BossClamp(x, BossArenaLeft + 80.0f, BossArenaRight - 80.0f);
				float y = look.y - 450.0f - 120.0f;

				BossBullet* bullet = acquireBullet();
				if (bullet != nullptr)
				{
					bullet->Charge(Vector2(x, y), BossRainScale);
					bullet->Launch(Vector2(0.0f, BossRainSpeed), RainBallDamage, BossRainHoming);
				}
				mCount++;
				if (mCount >= BossRainCount)
					setStep(2);
			}
			break;
		case 2:
			if (mStepTime >= 2.6f)
				enterState(eBossState::Groggy);
			break;
		default:
			break;
		}
	}

	// ---------------------------------------------------------------------------------------------
	// 양손 3연타(2페이즈): 바닥이 붉게 빛나 예고한 뒤 두 주먹으로 내려친다. 3번, 세 번째는 늦게 들어온다.
	// 예고 구역은 내려치기 직전마다 그때의 플레이어 위치로 다시 잡는다

	void Chapter1_Boss::tripleSlam()
	{
		switch (mStep)
		{
		case 0:
			if (mbStepEnter)
			{
				mBothFistRoar->Play(false);
				Camera::Shake(5.0f, 0.8f);
				setStep(1);
			}
			break;
		case 1: // 들어 올리고 예고
		{
			if (mbStepEnter)
			{
				mSlamCenterX = BossClamp(playerPos().x, BossArenaLeft + 450.0f, BossArenaRight - 450.0f);
				mTelegraph->Show(mSlamCenterX);
				float quarter = BossTelegraph::Width / 4.0f;
				for (bool right : { false, true })
				{
					Boss_Hand* h = hand(right);
					h->SetPose(Boss_Hand::ePose::Fist);
					h->SetAttack(0);
					h->MoveTo(Vector2(mSlamCenterX + (right ? quarter : -quarter), BossTripleRaisedY), 2200.0f);
					h->SetTremble(true);
				}
			}

			float raise = BossTripleRaise + (mCount == 2 ? BossTripleLateExtra : 0.0f);
			if (mStepTime >= raise && mLeftHand->IsArrived() && mRightHand->IsArrived())
			{
				// 두 주먹이 한 번의 공격이다: 두 손에 동시에 닿아도 피해는 한 번만
				for (bool right : { false, true })
				{
					Boss_Hand* h = hand(right);
					Vector2 pos = h->GetPos();
					h->SetTremble(false);
					h->SetAttack(TripleSlamDamage, hand(!right));
					h->MoveTo(Vector2(pos.x, BossFistFloorY), BossTripleSlamSpeed);
				}
				setStep(2);
			}
			break;
		}
		case 2: // 내려치는 중
			if (mLeftHand->IsArrived() && mRightHand->IsArrived())
			{
				mTelegraph->Hide();
				mFistSlam->Play(false);
				Camera::Shake(16.0f, 0.45f);
				playEffect((int)BossEffect::eType::SlamSmoke, Vector2(mLeftHand->GetPos().x, BossDustY));
				playEffect((int)BossEffect::eType::SlamSmoke, Vector2(mRightHand->GetPos().x, BossDustY));
				setStep(3);
			}
			break;
		case 3:
			if (mStepTime >= 0.12f)
			{
				mLeftHand->SetAttack(0);
				mRightHand->SetAttack(0);
			}
			if (mStepTime >= BossTripleStay)
			{
				mCount++;
				if (mCount < 3)
				{
					setStep(1);
				}
				else
				{
					mFistSlamRecovery->Play(false);
					handsHome(1400.0f);
					setStep(4);
				}
			}
			break;
		case 4:
			if ((mLeftHand->IsArrived() && mRightHand->IsArrived()) || mStepTime >= 1.0f)
				enterState(eBossState::Idle);
			break;
		default:
			break;
		}
	}

	// ---------------------------------------------------------------------------------------------
	// 사망: 폭발 연기와 함께 쓰러지고, 화면이 어두워졌다 밝아지면 마석이 빠진(제정신) 모습으로 엎드려 있다.
	// 보상 상자·엔딩 흐름은 Phase 3에서 붙인다

	void Chapter1_Boss::dead()
	{
		static const float explodeTimes[6] = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f, 1.3f };
		static const Vector2 explodeOffsets[6] =
		{
			Vector2(0.0f, 0.0f), Vector2(-260.0f, 120.0f), Vector2(240.0f, 80.0f),
			Vector2(-120.0f, -150.0f), Vector2(180.0f, -120.0f), Vector2(0.0f, 200.0f),
		};

		if (mbStepEnter && mStep == 0)
		{
			mbGroggy = false;
			stopAllBullets();
			mTelegraph->Hide();
			for (bool right : { false, true })
			{
				Boss_Hand* h = hand(right);
				h->SetAttack(0);
				h->SetTremble(false);
			}
			handsRest(800.0f);
			setPartsOffset(Vector2(0.0f, BossCollapseY), 200.0f);
			mHead->SetBob(false);
			mChin->SetOpen(false);
			mDeadImpact->Play(false);
			Camera::Shake(18.0f, 1.2f);
		}

		// 폭발: mCount번째 폭발 시간이 되면 하나씩
		while (mCount < 6 && mStateTime >= explodeTimes[mCount])
		{
			Vector2 pos = mHead->GetGemPos() + explodeOffsets[mCount];
			pos.y += 160.0f;	// 이펙트 위치는 아래쪽 가운데라 폭발 가운데가 pos 근처에 오도록 내린다
			playEffect((int)BossEffect::eType::DeathExplosion, pos);
			mCount++;
		}

		// 나머지 연출은 시간 순서대로 한 번씩
		if (mStep == 0 && mStateTime >= 0.4f)
		{
			mDeadRoar->Play(false);
			mStep = 1;
		}
		else if (mStep == 1 && mStateTime >= 1.6f)
		{
			Camera::FadeOut(0.7f);
			mStep = 2;
		}
		else if (mStep == 2 && mStateTime >= 2.4f)
		{
			// 화면이 검은 동안 원래(보라색이 아닌) 모습 + 제정신을 찾은 머리로 바꾼다
			setPartsPhase2(false);
			mHead->SetLook(Boss_Head::eLook::Sane);
			mbBarVisible = false;
			Camera::FadeIn(1.4f);
			mStep = 3;
		}
	}

	// ---------------------------------------------------------------------------------------------
	// 부품 조작 도우미

	Boss_Hand* Chapter1_Boss::hand(bool right)
	{
		return right ? static_cast<Boss_Hand*>(mRightHand) : mLeftHand;
	}

	Vector2 Chapter1_Boss::hoverHome(bool right)
	{
		return Vector2(mCenterX + (right ? BossHandSpread : -BossHandSpread), BossHoverY);
	}

	Vector2 Chapter1_Boss::raisedPos(bool right)
	{
		return Vector2(mCenterX + (right ? BossHandSpread : -BossHandSpread), BossRaisedY);
	}

	Vector2 Chapter1_Boss::restPos(bool right)
	{
		return Vector2(mCenterX + (right ? BossHandSpread - 10.0f : -BossHandSpread + 10.0f), BossFlatY);
	}

	Vector2 Chapter1_Boss::sweepOutside(bool right)
	{
		return Vector2(right ? BossArenaRight + BossSweepOutside : BossArenaLeft - BossSweepOutside, BossFistFloorY);
	}

	void Chapter1_Boss::handsHome(float speed)
	{
		for (bool right : { false, true })
		{
			Boss_Hand* h = hand(right);
			h->SetPose(Boss_Hand::ePose::Hover);
			h->SetTremble(false);
			h->SetAttack(0);
			h->MoveTo(hoverHome(right), speed);
		}
	}

	void Chapter1_Boss::handsRest(float speed)
	{
		for (bool right : { false, true })
		{
			Boss_Hand* h = hand(right);
			Vector2 pos = h->GetPos();
			if (pos.x < BossArenaLeft || pos.x > BossArenaRight)
				h->Teleport(sweepOutside(right));
			h->SetPose(Boss_Hand::ePose::Flat);
			h->SetTremble(false);
			h->SetAttack(0);
			h->MoveTo(restPos(right), speed);
		}
	}

	void Chapter1_Boss::setPartsOffset(Vector2 offset, float speed)
	{
		mBody->SetOffset(offset, speed);
		mHead->SetOffset(offset, speed);
	}

	void Chapter1_Boss::setPartsPhase2(bool phase2)
	{
		mBody->SetLook(phase2 ? Boss_Body::eLook::Phase2 : Boss_Body::eLook::Normal);
		mHead->SetLook(phase2 ? Boss_Head::eLook::Phase2 : Boss_Head::eLook::Normal);
		mChin->SetPhase2(phase2);
		mLeftHand->SetPhase2(phase2);
		mRightHand->SetPhase2(phase2);
	}

	void Chapter1_Boss::stopAllBullets()
	{
		for (BossBullet* bullet : mBullets)
		{
			if (bullet != nullptr && !bullet->IsFree())
				bullet->Pop();
		}
		mBigBall = nullptr;
	}

	BossBullet* Chapter1_Boss::acquireBullet()
	{
		for (BossBullet* bullet : mBullets)
		{
			if (bullet != nullptr && bullet->IsFree())
				return bullet;
		}
		return nullptr;
	}

	BossEffect* Chapter1_Boss::playEffect(int type, Vector2 bottomCenter)
	{
		for (BossEffect* effect : mEffects)
		{
			if (effect != nullptr && effect->IsFree())
			{
				effect->Play((BossEffect::eType)type, bottomCenter);
				return effect;
			}
		}
		return nullptr;
	}

	Vector2 Chapter1_Boss::playerPos()
	{
		PlayeScene* scene = dynamic_cast<PlayeScene*>(SceneManager::GetActiveScene());
		Skul* skul = (scene != nullptr) ? scene->GetSkul() : nullptr;
		if (skul == nullptr)
			return Vector2(mCenterX, 1726.0f);

		return skul->GetComponent<Transform>()->GetPos();
	}
}
