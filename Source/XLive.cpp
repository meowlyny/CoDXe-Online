#include "XLive.h"

CDetour Live_IsUserSignedInToDemonware_Hook;
static BOOL Live_IsUserSignedInToDemonware(int controllerIndex)
{
	return 2;
}

CDetour Live_IsUserSignedInToLive_Hook;
static BOOL Live_IsUserSignedInToLive(int controllerIndex)
{
	return 2;
}

CDetour Live_Base_IsConnected_Hook;
static BOOL Live_Base_IsConnected(int controllerIndex)
{
	return 5;
}

CDetour LiveStorage_DoWeHaveCurrentStats_Hook;
static bool LiveStorage_DoWeHaveCurrentStats(int controllerIndex)
{
	static bool hasBeenCalled = false;

	if (!hasBeenCalled)
	{
		hasBeenCalled = true;
		return false;
	}

	return true;
}

CDetour LiveStorage_DoWeHaveStats_Hook;
static bool LiveStorage_DoWeHaveStats(int controllerIndex, int playerStatsLocation)
{
	return true;
}

CDetour LiveStorage_ValidateFFOTD_Hook;
static bool LiveStorage_ValidateFFOTD()
{
	return true;
}

CDetour LiveStorage_DoWeHavePlaylists_Hook;
static bool LiveStorage_DoWeHavePlaylists()
{
	return true;
}

CDetour LiveStorage_DoWeHaveAllStats_Hook;
static bool LiveStorage_DoWeHaveAllStats(int controllerIndex)
{
	return true;
}

CDetour LiveStorage_IsTimeSynced_Hook;
static bool LiveStorage_IsTimeSynced()
{
	return true;
}

CDetour LiveStorage_DoWeHaveContracts_Hook;
static bool LiveStorage_DoWeHaveContracts()
{
	return true;
}

CDetour SanityCheckSession_Hook;
static void SanityCheckSession(const char** unknown)
{
	return;
}

CDetour LiveStats_GetClanTagText_Hook;
static const char* LiveStats_GetClanTagText(int controllerIndex)
{
	return "3arc"; // funniness
}

CDetour LiveGroups_GetCount_Hook;
static unsigned int LiveGroups_GetCount(const char* path)
{
	return 999; // lol
}

CDetour LiveStorage_CompletedPlaylistPopulation_Hook;
static int LiveStorage_CompletedPlaylistPopulation()
{
	return true;
}

void CXLive::RegisterHooks()
{
	Live_IsUserSignedInToDemonware_Hook.CreateDetour(0x827097F0, Live_IsUserSignedInToDemonware);
	Live_IsUserSignedInToLive_Hook.CreateDetour(0x82710A60, Live_IsUserSignedInToLive);
	Live_Base_IsConnected_Hook.CreateDetour(0x82769478, Live_Base_IsConnected);
	LiveStorage_DoWeHaveCurrentStats_Hook.CreateDetour(0x82754A48, LiveStorage_DoWeHaveCurrentStats);
	LiveStorage_DoWeHaveStats_Hook.CreateDetour(0x827549E8, LiveStorage_DoWeHaveStats);
	LiveStorage_ValidateFFOTD_Hook.CreateDetour(0x82760170, LiveStorage_ValidateFFOTD);
	LiveStorage_DoWeHavePlaylists_Hook.CreateDetour(0x8275F398, LiveStorage_DoWeHavePlaylists);
	LiveStorage_DoWeHaveAllStats_Hook.CreateDetour(0x82754AB0, LiveStorage_DoWeHaveAllStats);
	LiveStorage_IsTimeSynced_Hook.CreateDetour(0x827563D0, LiveStorage_IsTimeSynced);
	LiveStorage_DoWeHaveContracts_Hook.CreateDetour(0x8275F3C8, LiveStorage_DoWeHaveContracts);
	SanityCheckSession_Hook.CreateDetour(0x8273E7E8, SanityCheckSession);
	//Utils::Hook::SetValue<uint8_t>(0x842EE640, 1); // show globe
	LiveStats_GetClanTagText_Hook.CreateDetour(0x82752BF8, LiveStats_GetClanTagText);
	LiveGroups_GetCount_Hook.CreateDetour(0x8272AD28, LiveGroups_GetCount);
	LiveStorage_CompletedPlaylistPopulation_Hook.CreateDetour(0x8275FC50, LiveStorage_CompletedPlaylistPopulation);
}
