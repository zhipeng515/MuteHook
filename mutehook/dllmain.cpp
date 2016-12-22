// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "mutehook.h"
#include "../../CommonFunction/Utility.h"
#include "../../MuteVolume/MuteVolume/MuteVolume.h"

DWORD WINAPI FreeSelfProc(PVOID param)
{
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	//TCHAR szModuleName[256];
	//HANDLE hProcess = GetCurrentProcess();
	//GetModuleFileName(NULL, szModuleName, 256);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
//		MessageBox(NULL, "DLL_PROCESS_ATTACH", "mutehook", MB_OK);
		MuteHook::Instance()->Init();
		MuteHook::Instance()->SetModuleHandle(hModule);
		break;
	case DLL_THREAD_ATTACH:
//		MessageBox(NULL, "DLL_THREAD_ATTACH", "mutehook", MB_OK);
//		MuteVolumeManager::Instance()->Mute(true);
		break;
	case DLL_THREAD_DETACH:
//		MessageBox(NULL, "DLL_THREAD_DETACH", "mutehook", MB_OK);
//		MuteVolumeManager::Instance()->Mute(false);
		break;
	case DLL_PROCESS_DETACH:
//		CoUninitialize();
//		MessageBox(NULL, "DLL_PROCESS_DETACH", "mutehook", MB_OK);
//		MuteHook::Instance()->Exit();
		break;
	}
	return TRUE;
}

