#include "yaInput.h"
#include "yaApplication.h"

extern ya::Application application;

namespace ya
{
	int ASCII[(UINT)eKeyCode::END] =
	{
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M',

		VK_UP, VK_DOWN, VK_LEFT ,VK_RIGHT,
		VK_LBUTTON, VK_RBUTTON,

		VK_SPACE, VK_SHIFT, VK_ESCAPE, VK_RETURN, VK_TAB,
		'1', '2', '3', '4',
	};

	// 시나리오 대본에서 쓰는 키 이름 (ASCII 배열과 같은 순서)
	const char* KeyNames[(UINT)eKeyCode::END] =
	{
		"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
		"A", "S", "D", "F", "G", "H", "J", "K", "L",
		"Z", "X", "C", "V", "B", "N", "M",

		"UP", "DOWN", "LEFT", "RIGHT",
		"LBUTTON", "RBUTTON",

		"SPACE", "SHIFT", "ESC", "ENTER", "TAB",
		"1", "2", "3", "4",
	};

	std::vector<Input::Key> Input::mKeys;
	Vector2 Input::mMousePos = Vector2::Zero;
	bool Input::mScripted = false;
	bool Input::mScriptedDown[(UINT)eKeyCode::END] = {};

	bool Input::FindKeyCode(const std::string& name, eKeyCode& keyCode)
	{
		for (UINT i = 0; i < (UINT)eKeyCode::END; i++)
		{
			if (name == KeyNames[i])
			{
				keyCode = (eKeyCode)i;
				return true;
			}
		}
		return false;
	}

	void Input::Initialize()
	{
		for (UINT i = 0; i < (UINT)eKeyCode::END; i++)
		{
			Key keyInfo;
			keyInfo.key = (eKeyCode)i;
			keyInfo.state = eKeyState::None;
			keyInfo.bPressed = false;

			mKeys.push_back(keyInfo);
		}
	}

	void Input::Update()
	{
		if (mScripted || GetFocus())
		{
			for (UINT i = 0; i < (UINT)eKeyCode::END; i++)
			{
				bool down = mScripted ? mScriptedDown[i] : (GetAsyncKeyState(ASCII[i]) & 0x8000) != 0;
				if (down)
				{
					// 이전 프레임에도 눌려 있었다
					if (mKeys[i].bPressed)
						mKeys[i].state = eKeyState::Pressed;
					else
						mKeys[i].state = eKeyState::Down;

					mKeys[i].bPressed = true;
				}
				else // 현재 프레임에 키가 눌려있지 않다.
				{
					// 이전 프레임에 내키가 눌려있엇다.
					if (mKeys[i].bPressed)
						mKeys[i].state = eKeyState::Up;
					else
						mKeys[i].state = eKeyState::None;

					mKeys[i].bPressed = false;
				}
			}

			POINT mousePos = {};
			GetCursorPos(&mousePos);

			ScreenToClient(application.GetHwnd(), &mousePos);
			mMousePos.x = (float)mousePos.x;
			mMousePos.y = (float)mousePos.y;

		}
		else
		{
			for (UINT i = 0; i < (UINT)eKeyCode::END; i++)
			{
				if (eKeyState::Down == mKeys[i].state
					|| eKeyState::Pressed == mKeys[i].state)
				{
					mKeys[i].state = eKeyState::Up;
				}
				else if (eKeyState::Up == mKeys[i].state)
				{
					mKeys[i].state = eKeyState::None;
				}

				mKeys[i].bPressed = false;
			}
		}
	}

	void Input::Render(HDC hdc)
	{

	}
}
