#pragma once

#include "../../CommonFunction/Singleton.h"

#if defined MUTEHOOK_EXPORTS
#define MUTEHOOK_API		extern "C" __declspec(dllexport)
#else
#define MUTEHOOK_API		__declspec(dllimport)
#endif

class MuteHook : public Singleton<MuteHook>
{
public:
	MuteHook();
	~MuteHook();
	void HandleEventThread();

	void SetModuleHandle(HMODULE hModule)
	{
		m_hModule = hModule;
	}
	void Exit()
	{
		SetEvent(m_hExitEvent);
	}

	HANDLE m_hMuteEvent;
	HANDLE m_hUnmuteEvent;

	HANDLE m_hMuteEndEvent;
	HANDLE m_hUnmuteEndEvent;

	HANDLE m_hExitEvent;

	HMODULE m_hModule;
};

MUTEHOOK_API bool MuteHook_IsMute();
MUTEHOOK_API void MuteHook_Mute(bool bMute);

MUTEHOOK_API void MuteHook_Unload();