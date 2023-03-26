#include "yaPlayer.h"

namespace ya
{
	Player::Player()
	{
		mSkul.resize((UINT)eSkulType::End);
	}
	Player::~Player()
	{
	}
	void Player::Initialize()
	{
		mSkul[(UINT)eSkulType::Basic] = new Skul;

	}
	void Player::Update()
	{
	}
	void Player::Render(HDC hdc)
	{
	}
	void Player::Release()
	{
	}
}
