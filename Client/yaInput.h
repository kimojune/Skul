#pragma once
#include "YamYamEngine.h"

namespace ya
{
	enum class eKeyCode
	{
		Q, W, E, R, T, Y, U, I, O, P,
		A, S, D, F, G, H, J, K, L,
		Z, X, C, V, B, N, M,

		UP,DOWN,LEFT,RIGHT,

		LBUTTON, RBUTTON,
		END,
	};

	enum class eKeyState
	{
		Down,
		Pressed,
		Up,
		None,
	};

	class Input
	{
	public:
		struct Key
		{
			eKeyCode key;
			eKeyState state;
			bool bPressed;
		};

		static void Initialize();
		static void Update();
		static void Render(HDC hdc);

		inline static eKeyState GetKeyState(eKeyCode keyCode)
		{
			return mKeys[(UINT)keyCode].state;
		};

		//GetKey()		키를 누르는 시간만큼 true를 반환
		//GetKeyDown()	키를 눌렀을 때, 딱 한번 true를 반환
		//GetKeyUp()	키를 누르다 땠을 때, 딱 한번 true를 반환

		static __forceinline bool GetKey(eKeyCode keyCode)
		{
			return mKeys[static_cast<UINT>(keyCode)].state == eKeyState::Pressed;
		}

		static __forceinline bool GetKeyDown(eKeyCode keyCode)
		{
			return mKeys[static_cast<UINT>(keyCode)].state == eKeyState::Down;
		}

		static __forceinline bool GetKeyUp(eKeyCode keyCode)
		{
			return mKeys[static_cast<UINT>(keyCode)].state == eKeyState::Up;
		}
		
		static __forceinline bool GetKeyNone(eKeyCode keyCode)
		{
			return mKeys[static_cast<UINT>(keyCode)].state == eKeyState::None;
		}

		static __forceinline Vector2 GetMousePos() { return mMousePos; }

		// 시나리오(자동 검증) 실행 중에는 키보드 대신 대본이 키 상태를 정한다.
		static void SetScripted(bool scripted) { mScripted = scripted; }
		static void SetScriptedKey(eKeyCode keyCode, bool down) { mScriptedDown[(UINT)keyCode] = down; }
		static bool FindKeyCode(const std::string& name, eKeyCode& keyCode);

	private:
		static std::vector<Key> mKeys;
		static Vector2 mMousePos;
		static bool mScripted;
		static bool mScriptedDown[(UINT)eKeyCode::END];
	};
}
