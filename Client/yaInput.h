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

		// 값이 배열 인덱스이므로 새 키는 항상 END 바로 앞에 추가한다.
		SPACE, SHIFT, ESC, ENTER, TAB,
		NUM1, NUM2, NUM3, NUM4, // 키보드 윗줄 숫자 키 (키패드 아님)
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
		// 대본 실행 중 마우스 위치 (창 클라이언트 좌표). 실제 마우스는 읽지 않으므로 이 값이 유지된다
		static void SetScriptedMousePos(Vector2 pos) { mMousePos = pos; }
		static bool FindKeyCode(const std::string& name, eKeyCode& keyCode);

	private:
		static std::vector<Key> mKeys;
		static Vector2 mMousePos;
		static bool mScripted;
		static bool mScriptedDown[(UINT)eKeyCode::END];
	};
}
