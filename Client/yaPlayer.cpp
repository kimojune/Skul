#include "yaPlayer.h"

namespace ya
{
	Player::Player()
	{
		
	}
	Player::~Player()
	{
	}
	void Player::Initialize()
	{
		mSkul.resize((UINT)eSkulType::End);
		
		mSkul[(UINT)eSkulType::Basic] = new Skul();

		for (Skul* skul : mSkul)
		{
			if (skul == nullptr)
				continue;

			skul->Initialize();
		}

		mActiveSkul = mSkul[(UINT)eSkulType::Basic];
	}
	void Player::Update()
	{
		mActiveSkul->Update();
	}
	void Player::Render(HDC hdc)
	{
		mActiveSkul->Render(hdc);

	}
	void Player::Release()
	{
		mActiveSkul->Release();

	}
}