#pragma once
#include "CommonInclude.h"


namespace ya
{
	class SoundManager
	{
	public:
		static bool Initialize();
		static LPDIRECTSOUND8 GetDevice() { return mSoundDevice; }
		static void SetMute(bool mute) { mMute = mute; }
		static bool IsMute() { return mMute; }

	private:
		static LPDIRECTSOUND8 mSoundDevice;
		static bool mMute;
	};
}