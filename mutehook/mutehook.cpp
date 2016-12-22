// mutehook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <tchar.h>
#include "mutehook.h"
#include <ObjBase.h>
#include "../../MuteVolume/MuteVolume/MuteVolume.h"
#include "../../CommonFunction/StdLog.h"

#ifndef _LIB
#ifdef _DEBUG
#pragma comment (lib, "../../../lib/Debug/MuteVolume.lib")
#else
#pragma comment (lib, "../../../lib/Release/MuteVolume.lib")
#endif
#endif

DWORD WINAPI HandleEventThread(LPVOID lpParameter)
{
	MuteHook* pMuteHook = (MuteHook*)lpParameter;
	pMuteHook->HandleEventThread();
	return 0;
}
 
bool MuteHook_IsMute()
{
	return MuteVolumeManager::Instance()->IsMuted();
}

void MuteHook_Mute(bool bMute)
{
	if(bMute)
	{
		SetEvent(MuteHook::Instance()->m_hMuteEvent);
		WaitForSingleObject(MuteHook::Instance()->m_hMuteEndEvent, INFINITE);
		ResetEvent(MuteHook::Instance()->m_hMuteEndEvent);
	}
	else
	{
		SetEvent(MuteHook::Instance()->m_hUnmuteEvent);
		WaitForSingleObject(MuteHook::Instance()->m_hUnmuteEndEvent, INFINITE);
		ResetEvent(MuteHook::Instance()->m_hUnmuteEndEvent);
	}
}

void MuteHook_Unload()
{
	MuteHook::Instance()->Uninit();
}

bool MuteHook::Init()
{
	m_hMuteEvent = CreateEvent(NULL, true, false, _T("MuteHook_MuteEvent"));
	m_hUnmuteEvent = CreateEvent(NULL, true, false, _T("MuteHook_UnmuteEvent"));
	m_hMuteEndEvent = CreateEvent(NULL, true, false, _T("MuteHook_MuteEndEvent"));
	m_hUnmuteEndEvent = CreateEvent(NULL, true, false, _T("MuteHook_UnmuteEndEvent"));
	m_hExitEvent = CreateEvent(NULL, false, false, _T("MuteHook_ExitEvent"));

	m_hModule = NULL;

	HANDLE hThread = CreateThread(NULL, 0, &::HandleEventThread, this, 0, 0);
	CloseHandle(hThread);

	return true;
}

void MuteHook::Uninit()
{
	SetEvent(m_hExitEvent);
}

void MuteHook::HandleEventThread()
{
	bool bRun = true;
	HANDLE Events[] = {
		m_hMuteEvent,
		m_hUnmuteEvent,
		m_hExitEvent
	};

	MuteVolumeManager::Instance()->Init();

	while (bRun)
	{
		DWORD waitObjects = WaitForMultipleObjects(sizeof(Events)/sizeof(HANDLE), Events, FALSE, INFINITE);
		switch (waitObjects)
		{
		case WAIT_OBJECT_0:		// MuteEvent
			MuteVolumeManager::Instance()->Mute(true);
			ResetEvent(m_hMuteEvent);
			SetEvent(m_hMuteEndEvent);
			break;
		case WAIT_OBJECT_0+1:	// UnmuteEvent
			MuteVolumeManager::Instance()->Mute(false);
			ResetEvent(m_hUnmuteEvent);
			SetEvent(m_hUnmuteEndEvent);
			break;
		case WAIT_OBJECT_0+2:	// ExitEvent
			bRun = false;
			break;
		default:
			bRun = false;
			break;
		}
	}
	CloseHandle(m_hMuteEvent);
	CloseHandle(m_hUnmuteEvent);
	CloseHandle(m_hMuteEndEvent);
	CloseHandle(m_hUnmuteEndEvent);
	CloseHandle(m_hExitEvent);

	MuteVolumeManager::Instance()->Uninit();

	// 在线程完成之后自动卸载动态库
	FreeLibraryAndExitThread(m_hModule, 123);
}