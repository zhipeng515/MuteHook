#pragma once

#include <tchar.h>
#include <Shlwapi.h>
#include "../../CommonFunction/Singleton.h"
#include "../../CommonFunction/Utility.h"
#include "../../CommonFunction/DetoursWrapper.h"
#include "../../CommonFunction/StdLog.h"

DETOURS_FUNC_DECLARE(BOOL, WINAPI, CreateProcess, 
	__in_opt    LPCTSTR lpApplicationName,
	__inout_opt LPTSTR lpCommandLine,
	__in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
	__in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in        BOOL bInheritHandles,
	__in        DWORD dwCreationFlags,
	__in_opt    LPVOID lpEnvironment,
	__in_opt    LPCTSTR lpCurrentDirectory,
	__in        LPSTARTUPINFO lpStartupInfo,
	__out       LPPROCESS_INFORMATION lpProcessInformation);

class MuteHookLoader : public Singleton<MuteHookLoader>
{
public:
	virtual bool Init()
	{
		return LoadMuteHook();
	}
	virtual void Uninit()
	{
		UnloadMuteHook();
	}

	typedef bool(*FuncDefine_MuteHook_Bool)();
	FuncDefine_MuteHook_Bool MuteHook_IsMute;

	typedef void(*FuncDefine_MuteHook_Mute)(bool);
	FuncDefine_MuteHook_Mute MuteHook_Mute;

	typedef void(*FuncDefine_MuteHook_Void)();
	FuncDefine_MuteHook_Void MuteHook_Unload;

	bool LoadMuteHook()
	{
		DETOURS_FUNC_ATTACH(CreateProcess);

		m_MuteHook = LoadLibrary((Utility::GetExePath() + _T("\\mutehook.dll")).c_str());
		if (m_MuteHook)
		{
			MuteHook_IsMute = (FuncDefine_MuteHook_Bool)GetProcAddress(m_MuteHook, "MuteHook_IsMute");
			MuteHook_Mute = (FuncDefine_MuteHook_Mute)GetProcAddress(m_MuteHook, "MuteHook_Mute");
			MuteHook_Unload = (FuncDefine_MuteHook_Void)GetProcAddress(m_MuteHook, "MuteHook_Unload");
		}

		return m_MuteHook && MuteHook_IsMute && MuteHook_Mute && MuteHook_Unload;
	}
	void UnloadMuteHook()
	{
		if (m_MuteHook != NULL)
		{
			MuteHook_Unload();
			m_MuteHook = NULL;

			DETOURS_FUNC_DETACH(CreateProcess);
		}
	}
	bool LoadMuteHookToProcess(DWORD dwProcessID)
	{
		TCHAR szDllPath[MAX_PATH];
		GetModuleFileName(NULL, szDllPath, MAX_PATH);
		PathRemoveFileSpec(szDllPath);
		lstrcat(szDllPath, _T("\\mutehook.dll"));
		return Utility::InjectDll(szDllPath, dwProcessID);
	}

private:
	HMODULE m_MuteHook;
};

DETOURS_FUNC_IMPLEMENT(BOOL, WINAPI, CreateProcess, 
	__in_opt    LPCTSTR lpApplicationName,
	__inout_opt LPTSTR lpCommandLine,
	__in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
	__in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in        BOOL bInheritHandles,
	__in        DWORD dwCreationFlags,
	__in_opt    LPVOID lpEnvironment,
	__in_opt    LPCTSTR lpCurrentDirectory,
	__in        LPSTARTUPINFO lpStartupInfo,
	__out       LPPROCESS_INFORMATION lpProcessInformation)
{
	BOOL bRet = DETOURS_FUNC_CALLREAL(CreateProcess,
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation
	);

	BOOL bInject = MuteHookLoader::Instance()->LoadMuteHookToProcess(lpProcessInformation->dwProcessId);

	return bRet && bInject;
}