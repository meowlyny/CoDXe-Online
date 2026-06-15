#include "BitDemon.h" // DemonWare patches
#include "XLive.h" // Xbox Live patches

#define GREENLIGHT_TITLE_ID 0x415608C3

static bool IsSupportedTitleId(DWORD titleId)
{
	switch (titleId)
	{
	case GREENLIGHT_TITLE_ID:
		return true;

	default:
		return false;
	}
}

static void MainThread()
{
	DWORD titleId;

	do
	{
		Sleep(75); // avoid CPU load
		titleId = XamGetCurrentTitleId();
	} while (!IsSupportedTitleId(titleId));

	Sleep(100);

	CBitDemon::RegisterHooks();
	CXLive::RegisterHooks();
}

static void OnAttachProcess()
{
	HANDLE threadHandle = nullptr;
	DWORD threadID = 0;

	DWORD ret = ExCreateThread(&threadHandle, 0, &threadID, XapiThreadStartup, LPTHREAD_START_ROUTINE(MainThread), nullptr, 0x2 | CREATE_SUSPENDED);

	if (threadHandle)
	{
		XSetThreadProcessor(threadHandle, 4);
		ResumeThread(threadHandle);
	}
}

BOOL WINAPI DllMain(HANDLE hInstance, DWORD dwReason, LPVOID)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		OnAttachProcess();
		break;

	case DLL_PROCESS_DETACH:
		// Xenia Netplay has no dashboard system so do nothing
		break;
	}

	return TRUE;
}
