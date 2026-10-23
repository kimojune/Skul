#pragma once
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>


#include <string>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <filesystem>

#pragma comment(lib, "Msimg32.lib")


#include <mmsystem.h>
#include <dsound.h>
#include <dinput.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")


#define TILE_SIZE_X 72 
#define TILE_SIZE_Y 72

// 맵 오른쪽 끝 값이 세 군데서 다르다. 게임 동작은 그대로 두고 기록만 해 둔다.
//  - GROUN_SIZE_X 6400 : 스컬이 걸어갈 수 있는 x 한계 (yaSkul.cpp)
//  - 3400              : 바닥 충돌체 너비, x 0~3400 (yaGround.cpp)
//  - 3350              : 카메라 오른쪽 경계 (스테이지 OnEnter 의 Camera::SetBounds)
#define GROUN_SIZE_X 6400