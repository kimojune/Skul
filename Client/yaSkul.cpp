#pragma once

#include "yaSkul.h"
#include "yaTime.h"
#include "yaSceneManager.h"
#include "yaInput.h"
#include "yaResources.h"
#include "yaTransform.h"
#include "yaAnimator.h"
#include "yaCollider.h"
#include "yaSkulHead.h"
#include "yaScene.h"
#include "yaObject.h"
#include "yaRigidbody.h"
#include "yaPlayeScene.h"
#include "yaCamera.h"
#include "yaSkulAttack.h"
#include "yaSound.h"
#include "yaPlayerStats.h"


namespace ya
{
	eDirection Skul::mLastPressedDirection = eDirection::Right;


	Skul::Skul()
		: mState(eSkulState::Idle)
		, AttackCount(0)
		, SecondDash(true)
		, SecondJump(true)
		, mbDashInvincible(false)
		, mHitStunTime(0.0f)
		, mKnockback(0.0f)
		, mDeadTime(0.0f)
	{
	}
	Skul::~Skul()
	{
	}

	void Skul::Initialize()
	{
		GameObject::Initialize();

		Transform* tr = GetComponent<Transform>();
		tr->SetScale(Vector2(2.5f, 2.5f));

		mDirect = eDirection::Right;


		mAnimator = GetComponent<Animator>();

		Default_Dash = Resources::Load<Sound>(L"Default_Dash", L"..\\Resources\\Sound\\Skul\\Default_Dash.wav");
		Skul_Jump_Atk = Resources::Load<Sound>(L"Skul_Jump_Atk", L"..\\Resources\\Sound\\Skul\\Skul_Jump_Atk.wav");
		Skul_Atk1 = Resources::Load<Sound>(L"Skul_Atk1", L"..\\Resources\\Sound\\Skul\\Skul_Atk 1.wav");
		Skul_Atk2 = Resources::Load<Sound>(L"Skul_Atk 2", L"..\\Resources\\Sound\\Skul\\Skul_Atk 2.wav");
		Skul_SkullThrowing = Resources::Load<Sound>(L"Skul_SkullThrowing", L"..\\Resources\\Sound\\Skul\\Skul_SkullThrowing.wav");
		Skul_Hit_1 = Resources::Load<Sound>(L"Skul_Hit_1", L"..\\Resources\\Sound\\Skul\\Skul_Hit 1 (Unused).wav");
		Default_Jump = Resources::Load<Sound>(L"Default_Jump", L"..\\Resources\\Sound\\Skul\\Default_Jump.wav");
		Default_Jump_Air = Resources::Load<Sound>(L"Default_Jump_Air", L"..\\Resources\\Sound\\Skul\\Default_Jump_Air.wav");
		Default_Dead = Resources::Load<Sound>(L"Default_Dead", L"..\\Resources\\Sound\\Skul\\Default_Dead.wav");


		mAnimator->GetStartEvent(L"LeftDash") = std::bind(&Skul::StartDash, this);
		mAnimator->GetCompleteEvent(L"LeftDash") = std::bind(&Skul::CompleteDash, this);
		mAnimator->GetEndEvent(L"LeftDash") = std::bind(&Skul::EndDash, this);

		mAnimator->GetStartEvent(L"RightDash") = std::bind(&Skul::StartDash, this);
		mAnimator->GetCompleteEvent(L"RightDash") = std::bind(&Skul::CompleteDash, this);
		mAnimator->GetEndEvent(L"RightDash") = std::bind(&Skul::EndDash, this);


		mAnimator->GetStartEvent(L"LeftJump") = std::bind(&Skul::StartJump, this);
		mAnimator->GetCompleteEvent(L"LeftJump") = std::bind(&Skul::CompleteJump, this);
		mAnimator->GetEndEvent(L"LeftJump") = std::bind(&Skul::EndJump, this);

		mAnimator->GetStartEvent(L"RightJump") = std::bind(&Skul::StartJump, this);
		mAnimator->GetCompleteEvent(L"RightJump") = std::bind(&Skul::CompleteJump, this);
		mAnimator->GetEndEvent(L"RightJump") = std::bind(&Skul::EndJump, this);


		mAnimator->GetStartEvent(L"LeftFall") = std::bind(&Skul::StartFall, this);
		mAnimator->GetCompleteEvent(L"LeftFall") = std::bind(&Skul::CompleteFall, this);
		mAnimator->GetEndEvent(L"LeftFall") = std::bind(&Skul::EndFall, this);

		mAnimator->GetStartEvent(L"RightFall") = std::bind(&Skul::StartFall, this);
		mAnimator->GetCompleteEvent(L"RightFall") = std::bind(&Skul::CompleteFall, this);
		mAnimator->GetEndEvent(L"RightFall") = std::bind(&Skul::EndFall, this);


		mAnimator->GetStartEvent(L"LeftAttackA") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"LeftAttackA") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"LeftAttackA") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"RightAttackA") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"RightAttackA") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"RightAttackA") = std::bind(&Skul::EndAttack, this);


		mAnimator->GetStartEvent(L"LeftAttackB") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"LeftAttackB") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"LeftAttackB") = std::bind(&Skul::EndAttack, this);

		mAnimator->GetStartEvent(L"RightAttackB") = std::bind(&Skul::StartAttack, this);
		mAnimator->GetCompleteEvent(L"RightAttackB") = std::bind(&Skul::CompleteAttack, this);
		mAnimator->GetEndEvent(L"RightAttackB") = std::bind(&Skul::EndAttack, this);


		mAnimator->GetStartEvent(L"LeftJumpAttack") = std::bind(&Skul::StartJumpAttack, this);
		mAnimator->GetCompleteEvent(L"LeftJumpAttack") = std::bind(&Skul::CompleteJumpAttack, this);
		mAnimator->GetEndEvent(L"LeftJumpAttack") = std::bind(&Skul::EndJumpAttack, this);

		mAnimator->GetStartEvent(L"RightJumpAttack") = std::bind(&Skul::StartJumpAttack, this);
		mAnimator->GetCompleteEvent(L"RightJumpAttack") = std::bind(&Skul::CompleteJumpAttack, this);
		mAnimator->GetEndEvent(L"RightJumpAttack") = std::bind(&Skul::EndJumpAttack, this);


		// 콜라이더 아래 변 = 발. 기본 스컬 Idle 스프라이트의 보이는 맨 아래 줄 바로 밑 (위치.y + 23)
		Collider* collider = AddComponent<Collider>();
		collider->SetCenter(Vector2(-20.0f, -27.0f));
		collider->SetSize(Vector2(50.0f, 50.0f));

		mRigidbody = AddComponent<Rigidbody>();
		mRigidbody->SetMass(1.0f);
		mRigidbody->SetGround(false);

		IsAttack = true;

		mState = eSkulState::Fall;
		mAnimator->Play(L"RightFall", false);

		// 시나리오는 시작할 때 변수 이름을 검사하므로 초깃값을 미리 내보낸다
		PlayerStats::Publish();

		//mAttack=object::Instantiate<SkulAttack>(eLayerType::Effect);
		//mAttack->setSkull(this);
	}


	void Skul::Update()
	{
		// 서 있는 동안은 점프 횟수와 공중 투척 제한을 되돌린다
		if (mRigidbody->GetGround() && mState != eSkulState::Fall_Dead)
			PlayerStats::OnGrounded();

		// 좌우를 같이 누를 때 어느 쪽으로 갈지 정하려고 마지막으로 누른 방향을 기억한다 (경직 중에도)
		if (Input::GetKeyDown(eKeyCode::LEFT))
			mLastPressedDirection = eDirection::Left;
		if (Input::GetKeyDown(eKeyCode::RIGHT))
			mLastPressedDirection = eDirection::Right;

		// 맞은 직후 잠깐은 입력을 받지 않는다 (상태 전환도 경직이 끝난 뒤에)
		if (mHitStunTime > 0.0f && mState != eSkulState::Fall_Dead)
		{
			updateHitStun();
			updateDashInvincible();
			GameObject::Update();
			return;
		}

		switch (mState)
		{
		case ya::Skul::eSkulState::Idle:
			Idle();
			break;
		case ya::Skul::eSkulState::Move:
			Move();
			break;
		case ya::Skul::eSkulState::Dash:
			Dash();
			break;
		case ya::Skul::eSkulState::Jump:
			Jump();
			break;
		case ya::Skul::eSkulState::Fall:
			Fall();
			break;
		case ya::Skul::eSkulState::Attack:
			Attack();
			break;
		case ya::Skul::eSkulState::JumpAttack:
			Jumpattack();
			break;
		case ya::Skul::eSkulState::SkillA:
			SkillA();
			break;
		case ya::Skul::eSkulState::SkillS:
			SkillS();
			break;
		case ya::Skul::eSkulState::ChangeSkill:
			ChangeSkill();
			break;
		case ya::Skul::eSkulState::Fall_Dead:
			FallDead();
			break;
		default:
			break;
		}

		updateDashInvincible();

		GameObject::Update();

	}

	void Skul::Render(HDC hdc)
	{
		// 쓰러지는 애니메이션(마지막은 빛으로 사라짐)이 끝나면 더 그리지 않는다
		if (mState == eSkulState::Fall_Dead)
		{
			Animation* animation = mAnimator->GetAnimaiton();
			if (animation != nullptr && animation->IsComplete())
				return;
		}

		GameObject::Render(hdc);
	}
	void Skul::Release()
	{
		GameObject::Release();

	}

	Skul* Skul::SwitchSkul(eSkulType type)
	{
		Transform* tr = GetComponent<Transform>();
		
		Skul* skul = mSkuls[(UINT)type];
		Scene* ActiveScene = SceneManager::GetActiveScene();
		//mSecondSkul = this;
		skul->GetComponent<Transform>()->SetPos(tr->GetPos());
		bool ground = this->GetComponent<Rigidbody>()->GetGround();;

		skul->GetComponent<Rigidbody>()->SetGround(ground);

		// 공중에서 교대해도 떨어지던 속도를 이어받는다. 대시 속도(4000)는 대시 마찰력과 짝이라 넘기지 않는다
		Vector2 velocity = mRigidbody->GetVelocity();
		if (mState == eSkulState::Dash)
			velocity.x = 0.0f;
		skul->GetComponent<Rigidbody>()->SetVelocity(velocity);

		PlayeScene* playscene = dynamic_cast<PlayeScene*> (ActiveScene);
		playscene->SetSkul(type);
		skul->SetDirect(mDirect);
		skul->SetSkulState(eSkulState::Idle);	// 공중이면 Idle에서 바로 Fall로 넘어간다


		switch (mDirect)
		{
		case eDirection::Left:
			skul->GetComponent<Animator>()->Play(L"LeftIdle", true);
			break;
		case eDirection::Right:
			skul->GetComponent<Animator>()->Play(L"RightIdle", true);
			break;
		}

		// 멈추는 스컬이 켜 둔 대시 무적이 남지 않게 한다
		mState = eSkulState::Idle;
		updateDashInvincible();
		mHitStunTime = 0.0f;

		SetState(eState::Pause);

		return skul;
	}

	void Skul::OnDamaged(int damage, Vector2 sourcePos)
	{
		if (mState == eSkulState::Fall_Dead)
			return;

		if (PlayerStats::IsDead())
		{
			Die();
			return;
		}

		// 피격 반응 (원작 넉백 기록 없음, 이 클론에서 정함): 짧은 경직 + 맞은 반대쪽으로 조금 밀림 + 소리 + 화면 흔들림.
		// 원작처럼 피격 후 무적은 없다. 붉은 화면 테두리는 HUD(PlayerHUD)가 HP가 줄어든 것을 보고 그린다.
		Vector2 pos = GetComponent<Transform>()->GetPos();
		mHitStunTime = HitStunTime;
		mKnockback = (sourcePos.x <= pos.x) ? KnockbackSpeed : -KnockbackSpeed;
		Skul_Hit_1->Play(false);
		Camera::Shake(6.0f, 0.15f);
	}

	void Skul::Die()
	{
		if (mState == eSkulState::Fall_Dead)
			return;

		mState = eSkulState::Fall_Dead;
		mDeadTime = 0.0f;
		mHitStunTime = 0.0f;
		updateDashInvincible();

		// 공중이면 그대로 떨어지되 옆으로는 멈춘다
		Vector2 velocity = mRigidbody->GetVelocity();
		velocity.x = 0.0f;
		mRigidbody->SetVelocity(velocity);
		mRigidbody->Setfriction(100.0f);

		playDirectional(L"LeftDead", L"RightDead", false);
		Default_Dead->Play(false);
	}

	void Skul::ResetForNewRun()
	{
		mDeadTime = 0.0f;
		mHitStunTime = 0.0f;
		mKnockback = 0.0f;
		AttackCount = 0;
		IsAttack = true;

		// Stage1 시작 위치·상태 (Stage1::Initialize와 같은 값)
		GetComponent<Transform>()->SetPos(Vector2(0.0f, 450.0f));
		mRigidbody->SetVelocity(Vector2::Zero);
		mRigidbody->SetGround(false);
		mRigidbody->Setfriction(100.0f);

		mDirect = eDirection::Right;
		mState = eSkulState::Fall;
		mAnimator->Play(L"RightFall", false);
		updateDashInvincible();
	}

	void Skul::updateHitStun()
	{
		float deltaTime = (float)Time::DeltaTime();
		mHitStunTime -= deltaTime;
		if (mHitStunTime < 0.0f)
			mHitStunTime = 0.0f;

		// 걷기와 같은 x 범위 안에서만 밀린다
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();
		pos.x += mKnockback * deltaTime;
		if (pos.x < 0.0f)
			pos.x = 0.0f;
		if (pos.x > GROUN_SIZE_X)
			pos.x = GROUN_SIZE_X;
		tr->SetPos(pos);
	}

	void Skul::updateDashInvincible()
	{
		// 원작: 대시 중 무적. 대시 상태에 들어가면 켜고 벗어나면 끈다 (다른 곳에서 켠 무적은 건드리지 않는다)
		bool dashing = (mState == eSkulState::Dash);
		if (dashing == mbDashInvincible)
			return;

		mbDashInvincible = dashing;
		PlayerStats::SetInvincible(dashing);
	}

	void Skul::playDirectional(const wchar_t* left, const wchar_t* right, bool loop)
	{
		if (mDirect == eDirection::Left)
			mAnimator->Play(left, loop);
		else
			mAnimator->Play(right, loop);
	}

	bool Skul::startDash()
	{
		// 원작: 대시 2회 충전 (PlayerStats::DashRechargeTime마다 1회씩 다시 찬다)
		if (!PlayerStats::UseDashCharge())
			return false;

		mState = eSkulState::Dash;
		playDirectional(L"LeftDash", L"RightDash", false);
		return true;
	}

	bool Skul::startJump()
	{
		// 원작: 2단 점프. 착지하면 OnGrounded로 횟수가 돌아온다
		if (!PlayerStats::UseJump())
			return false;

		mState = eSkulState::Jump;
		playDirectional(L"LeftJump", L"RightJump", true);
		return true;
	}

	int Skul::moveInput()
	{
		// 둘 다 누르고 있으면 나중에 누른 쪽으로 간다 (→를 누른 채 ←를 눌러 돌아서는 경우).
		// 전에는 둘 다 누르면 Idle에 묶여 점프·공격·대시 입력까지 모두 무시됐다
		bool left = Input::GetKey(eKeyCode::LEFT);
		bool right = Input::GetKey(eKeyCode::RIGHT);
		if (left && right)
			return (mLastPressedDirection == eDirection::Left) ? -1 : 1;
		if (left)
			return -1;
		if (right)
			return 1;
		return 0;
	}

	void Skul::enterIdleAndHandleInput()
	{
		mState = eSkulState::Idle;
		playDirectional(L"LeftIdle", L"RightIdle", true);
		Idle();
	}

	void Skul::SetDirect(eDirection direct)
	{
		mDirect = direct;
	}

	bool Skul::CheckAnimation(const std::wstring& name)
	{
		Animation* animation = mAnimator->FindAnimation(name);

		if (animation == nullptr)
			return false;

		return true;
	}

	void Skul::OnCollisionEnter(Collider* other)
	{
	}

	void Skul::OnCollisionStay(Collider* other)
	{
	}

	void Skul::OnCollisionExit(Collider* other)
	{
	}



	/*key


	Z 대쉬
	X 공격
	C 점프
	A 스킬1
	S 스킬1
	D 정수
	F 상호작용
	SPACE 교대



	*/


	void Skul::Idle()
	{
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();

		// 전에는 아래 입력 처리 전체가 "좌우를 같이 누르고 있지 않을 때"로 감싸여 있어서,
		// 돌아서는 동안(두 키가 겹치는 동안) 점프·공격·대시가 무시되고 떨어지지도 않았다
		{
			int move = moveInput();
			if (move < 0)
			{
				mState = eSkulState::Move;
				mDirect = eDirection::Left;

				mAnimator->Play(L"LeftRun", true);

			}
			else if (move > 0)
			{
				mState = eSkulState::Move;
				mDirect = eDirection::Right;

				mAnimator->Play(L"RightRun", true);

			}

			if (!(mRigidbody->GetGround()))
			{
				mState = eSkulState::Fall;


				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftFall", false);
					break;
				case eDirection::Right:
					mAnimator->Play(L"RightFall", false);
					break;
				default:
					break;
				}


			}




			if (Input::GetKeyDown(eKeyCode::Z))
			{
				startDash();
			}



			if (Input::GetKeyDown(eKeyCode::X))
			{
				mState = eSkulState::Attack;
				SecondAttack = true;

				switch (mDirect)
				{
				case eDirection::Left:
					mAnimator->Play(L"LeftAttackA", false);
					break;

				case eDirection::Right:
					mAnimator->Play(L"RightAttackA", false);
					break;

				default:
					break;
				}

			}



			if (Input::GetKeyDown(eKeyCode::C))
			{
				startJump();
			}


			if (Input::GetKeyDown(eKeyCode::A) && CanUseSkillA())
			{
				mState = eSkulState::SkillA;
			}



			if (Input::GetKeyDown(eKeyCode::S) && CanUseSkillS())
			{
				mState = eSkulState::SkillS;
			}

			if (Input::GetKeyDown(eKeyCode::D))
			{
				//Skul* skul = SwitchSkul(mSecondSkul);
				mState = eSkulState::Idle;
				if (mDirect == eDirection::Left)
					mAnimator->Play(L"LeftIdle", true);
				else
					mAnimator->Play(L"RightIdle", true);
			}

		}
	}
	void Skul::Move()
	{

		if (Input::GetKeyNone(eKeyCode::LEFT)
			&& Input::GetKeyNone(eKeyCode::RIGHT))
		{
			mState = eSkulState::Idle;


			switch (mDirect)
			{
			case eDirection::Left:
			{
				mAnimator->Play(L"LeftIdle", true);
				break;
			}
			case eDirection::Right:
			{
				mAnimator->Play(L"RightIdle", true);
				break;
			}
			}

		}



		// 좌우를 같이 누르면 전에는 Idle로 바뀌어 모든 입력이 막혔다. 이제는 나중에 누른 쪽으로 계속 걷는다 (moveInput)


		if (Input::GetKeyDown(eKeyCode::Z))
		{
			startDash();
		}





		if (Input::GetKeyDown(eKeyCode::X))
		{
			mState = eSkulState::Attack;
			SecondAttack = true;

			switch (mDirect)
			{
			case eDirection::Left:
				mAnimator->Play(L"LeftAttackA", false);
				break;

			case eDirection::Right:
				mAnimator->Play(L"RightAttackA", false);
				break;

			default:
				break;
			}


		}



		if (Input::GetKeyDown(eKeyCode::C))
		{
			startJump();
		}



		if (Input::GetKeyDown(eKeyCode::A) && CanUseSkillA())
		{
			mState = eSkulState::SkillA;
		}



		if (Input::GetKeyDown(eKeyCode::S) && CanUseSkillS())
		{
			mState = eSkulState::SkillS;
		}


		//이동 부
		if (mState == eSkulState::Move)
		{
			Transform* tr = GetComponent<Transform>();
			Vector2 pos = tr->GetPos();
			Animation* mAnimation = mAnimator->GetAnimaiton();
			std::wstring mAnimationName = mAnimation->GetAnimationName();
			int move = moveInput();

			if (move < 0)
			{
				mDirect = eDirection::Left;

				if (pos.x >= 0)
					pos.x -= 200.0f * Time::DeltaTime();

				if (!(mAnimationName == L"LeftRun"))
				{
					mAnimator->Play(L"LeftRun", true);
				}
				//mRigidbody->AddForce(Vector2(-200.0f, 0.0f));
			}

			if (move > 0)
			{
				mDirect = eDirection::Right;
				if (pos.x <= GROUN_SIZE_X)
				pos.x += 200.0f * Time::DeltaTime();

				//if (pos.x >= 0)
				//	pos.x -= 200.0f * Time::DeltaTime();
				//mRigidbody->AddForce(Vector2(200.0f, 0.0f));

				if (!(mAnimationName == L"RightRun"))
				{

					mAnimator->Play(L"RightRun", true);

				}
			}
			tr->SetPos(pos);

		}


	}

	void Skul::Dash()
	{
		Vector2 velocity = mRigidbody->GetVelocity();

		velocity.y = 0;

		mRigidbody->SetVelocity(velocity);

		// 대시가 끝난 프레임에 눌린 키도 받는다
		if (velocity.x == 0)
			enterIdleAndHandleInput();
	}
	void Skul::Jump()
	{
		Vector2 mVelocity = mRigidbody->GetVelocity();

		if (mVelocity.y >= 0)

		{
			mState = eSkulState::Fall;


			switch (mDirect)
			{
			case eDirection::Left:
				mAnimator->Play(L"LeftFall", false);
				break;
			case eDirection::Right:
				mAnimator->Play(L"RightFall", false);
				break;
			default:
				break;
			}

		}
		if (Input::GetKeyDown(eKeyCode::Z))
		{
			startDash();
		}


		if (Input::GetKeyDown(eKeyCode::X))
		{
			mState = eSkulState::JumpAttack;


			switch (mDirect)
			{
			case eDirection::Left:
				mAnimator->Play(L"LeftJumpAttack", false);
				break;

			case eDirection::Right:
				mAnimator->Play(L"RightJumpAttack", false);
				break;

			default:
				break;
			}

		}

		if (Input::GetKeyDown(eKeyCode::C))
		{
			startJump();
		}

		if (Input::GetKeyDown(eKeyCode::A) && CanUseSkillA())
		{
			mState = eSkulState::SkillA;
		}



		if (Input::GetKeyDown(eKeyCode::S) && CanUseSkillS())
		{
			mState = eSkulState::SkillS;
		}



		//이동 부

		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();
		int move = moveInput();

		if (move < 0)
		{
			if (pos.x >= 0)
			pos.x -= 200.0f * Time::DeltaTime();
			//mRigidbody->AddForce(Vector2(-200.0f, 0.0f));
		}

		if (move > 0)
		{
			if(pos.x<= GROUN_SIZE_X)
			pos.x += 200.0f * Time::DeltaTime();
			//mRigidbody->AddForce(Vector2(200.0f, 0.0f));

		}
		tr->SetPos(pos);

	}
	void Skul::Fall()
	{
		Rigidbody* rd = GetComponent<Rigidbody>();

		if (rd->GetGround())
		{
			SecondDash = true;
			SecondJump = true;

			// 착지한 프레임에 눌린 키는 지상 입력으로 받는다 (전에는 X가 공중 공격이 되어 바로 끝나고, 착지 프레임 입력이 사라졌다)
			enterIdleAndHandleInput();
			return;
		}


		if (Input::GetKeyDown(eKeyCode::Z))
		{
			startDash();
		}



		if (Input::GetKeyDown(eKeyCode::X))
		{
			mState = eSkulState::JumpAttack;


			switch (mDirect)
			{
			case eDirection::Left:
				mAnimator->Play(L"LeftJumpAttack", false);
				break;

			case eDirection::Right:
				mAnimator->Play(L"RightJumpAttack", false);
				break;

			default:
				break;
			}
		}

		if (Input::GetKeyDown(eKeyCode::C))
		{
			startJump();
		}

		if (Input::GetKeyDown(eKeyCode::A) && CanUseSkillA())
		{
			mState = eSkulState::SkillA;
		}



		if (Input::GetKeyDown(eKeyCode::S) && CanUseSkillS())
		{
			mState = eSkulState::SkillS;
		}


		//이동 부
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();
		int move = moveInput();

		if (move < 0)
		{
			if(pos.x >= 0)
			pos.x -= 100.0f * Time::DeltaTime();
			/*		mRigidbody->AddForce(Vector2(-200.0f, 0.0f));*/
		}

		if (move > 0)
		{
			if(pos.x <= GROUN_SIZE_X)
			pos.x += 100.0f * Time::DeltaTime();
			/*		mRigidbody->AddForce(Vector2(200.0f, 0.0f));*/

		}
		tr->SetPos(pos);

	}

	void Skul::Attack()
	{
		if (AttackCount < 1 && Input::GetKeyDown(eKeyCode::X))
		{
			AttackCount = 1;
		}

		Animation* mAnimation = mAnimator->GetAnimaiton();

		if (mAnimation->IsComplete())
		{
			if (AttackCount > 0)
			{
				AttackCount = 0;

				if (SecondAttack)
				{
					SecondAttack = false;


					switch (mDirect)
					{

					case eDirection::Left:
						mAnimator->Play(L"LeftAttackB", false);
						break;

					case eDirection::Right:
						mAnimator->Play(L"RightAttackB", false);
						break;

					default:
						break;
					}

				}
				else
				{
					SecondAttack = true;
					AttackCount = 0;


					switch (mDirect)
					{

					case eDirection::Left:
						mAnimator->Play(L"LeftAttackA", false);
						break;

					case eDirection::Right:
						mAnimator->Play(L"RightAttackA", false);
						break;

					default:
						break;
					}

				}
			}

			else
			{
				// 공격이 끝난 프레임에 눌린 점프·대시 등도 받는다
				enterIdleAndHandleInput();
			}

		}


	}
	void Skul::SkillA()
	{

	}
	void Skul::SkillS()
	{
	}

	void Skul::ChangeSkill()
	{
		mState = eSkulState::Idle;
		if (mDirect == eDirection::Left)
			mAnimator->Play(L"LeftIdle", true);
		else
			mAnimator->Play(L"RightIdle", true);
	}

	void Skul::Jumpattack()
	{
		Rigidbody* rd = GetComponent<Rigidbody>();

		if (rd->GetGround())
		{
			// 착지한 프레임에 눌린 키도 받는다
			enterIdleAndHandleInput();
		}

	}

	void Skul::FallDead()
	{
		// 입력은 받지 않는다. 공중이면 Rigidbody가 떨어뜨린다
		mDeadTime += (float)Time::DeltaTime();

		// 끝나기 DeadFadeTime 전부터 화면을 어둡게 한다 (타이틀로 넘어간 뒤 PlayeScene이 FadeIn)
		float fadeStart = DeadTime - DeadFadeTime;
		float prevTime = mDeadTime - (float)Time::DeltaTime();
		if (prevTime < fadeStart && mDeadTime >= fadeStart)
			Camera::FadeOut(DeadFadeTime);
	}


	// Event
	void Skul::StartDash()
	{
		Default_Dash->Play(false);

		Vector2 velocity = mRigidbody->GetVelocity();
		Transform* tr = GetComponent<Transform>();
		Vector2 pos = tr->GetPos();

		if (mDirect == eDirection::Left)
		{
			velocity.x = -4000;
			velocity.y = 0;
		}

		else if (mDirect == eDirection::Right)
		{
			velocity.x = 4000;
			velocity.y = 0;
		}
		mRigidbody->Setfriction(30000.0f);
		mRigidbody->SetVelocity(velocity);

	}

	void Skul::CompleteDash()
	{

	}

	void Skul::EndDash()
	{
		mRigidbody->Setfriction(100.0f);
	}



	void Skul::StartSkillA()
	{

	}

	void Skul::CompleteSkillA()
	{
		if (mDirect == eDirection::Left)
			mAnimator->Play(L"LeftIdle", true);

		else if (mDirect == eDirection::Right)
			mAnimator->Play(L"RightIdle", true);

		mState = eSkulState::Idle;
	}

	void Skul::EndSkillA()
	{
	}



	void Skul::StartAttack()
	{
		if (SecondAttack)
			Skul_Atk1->Play(false);
		else
			Skul_Atk2->Play(false);

		IsAttack = true;
	}

	void Skul::CompleteAttack()
	{
		
	}

	void Skul::EndAttack()
	{
	}

	void Skul::StartJumpAttack()
	{
		Skul_Jump_Atk->Play(false);

		// 공중 공격마다 한 번 맞힐 수 있게 다시 켠다. 전에는 착지로 끊긴 공중 공격이 맞힌 뒤
		// 다음 지상 공격 전까지 꺼진 채라 그다음 공중 공격이 피해를 주지 못했다
		IsAttack = true;
	}

	void Skul::CompleteJumpAttack()
	{
		mState = eSkulState::Fall;

		switch (mDirect)
		{
		case eDirection::Left:
			mAnimator->Play(L"LeftFall", false);
			break;
		case eDirection::Right:
			mAnimator->Play(L"RightFall", false);
			break;
		default:
			break;
		}

		IsAttack = true;
	}

	void Skul::EndJumpAttack()
	{
	}


	void Skul::StartJump()
	{
		// 두 번째(공중) 점프는 공중 점프 소리
		if (PlayerStats::GetJumpCount() >= 2)
			Default_Jump_Air->Play(false);
		else
			Default_Jump->Play(false);

		// 위로 600px/s. 전에는 지금 속도에서 600을 빼서, 올라가는 중에 2단 점프하면 더 높이,
		// 떨어지는 중에 하면 거의 뜨지 않았다. 땅에서 뛰는 첫 점프(속도 0)는 전과 같다
		Vector2 velocity = mRigidbody->GetVelocity();
		velocity.y = -600.0f;

		mRigidbody->SetVelocity(velocity);
		mRigidbody->SetGround(false);
	}

	void Skul::CompleteJump()
	{

	}
	void Skul::EndJump()
	{
	}

	void Skul::StartFall()
	{

	}

	void Skul::CompleteFall()
	{

		switch (mDirect)
		{
		case eDirection::Left:
			mAnimator->Play(L"LeftFallRepeat", true);
			break;
		case eDirection::Right:
			mAnimator->Play(L"RightFallRepeat", true);
			break;
		default:
			break;
		}


	}

	void Skul::EndFall()
	{

	}



	void Skul::StartSkillS()
	{


	}

	void Skul::CompleteSkillS()
	{
	}

	void Skul::EndSkillS()
	{
	}
	void Skul::StartChangeSkill()
	{
	}
	void Skul::CompleteChangeSkill()
	{
		
	}
	void Skul::EndChangeSkill()
	{
	}
}