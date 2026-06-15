#include "BitDemon.h"
#include <xgraphics.h>

bool fetchCompleted = (bool)0x84C576A8;

uint8_t g_playlistPopulationBuffer[0x10000];
uint8_t g_playlistPopulationDecompBuffer[0x40000];
DWORD g_playlistPopulationBufferSize = 0;

CDetour LiveStorage_FetchPlaylistPopulation_Detour;
/*TaskRecord*/int LiveStorage_FetchPlaylistPopulation(int controllerIndex)
{
	HANDLE hFile = CreateFile("game:\\demonware\\publisher\\largeheatmapcopy.raw", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	DWORD fileSize = GetFileSize(hFile, NULL);
	DWORD bytesRead = 0;
	ReadFile(hFile, g_playlistPopulationBuffer, 0x10000, &bytesRead, NULL);
	CloseHandle(hFile);

	if (bytesRead == 0)
		return 0;

	g_playlistPopulationBufferSize = bytesRead;

	/*s_playlistPopulationFileInfo.*/fetchCompleted = true;

	return 1;
}

CDetour LiveStorage_GetPlaylistPopulationBuffer_Detour;
uint8_t* LiveStorage_GetPlaylistPopulationBuffer()
{
	return g_playlistPopulationBuffer;
}

CDetour UI_GeneratePlaylistPopulationTextureInternal_Detour;
void UI_GeneratePlaylistPopulationTextureInternal(/*GfxImage*/int image, uint8_t* buffer)
{
	z_stream stream = { 0 };
	stream.next_in = buffer;
	stream.avail_in = g_playlistPopulationBufferSize;
	stream.next_out = g_playlistPopulationDecompBuffer;
	stream.avail_out = 0x40000;

	inflateInit2(&stream, 15);
	int result = inflate(&stream, Z_FINISH);
	inflateEnd(&stream);

	if (result != Z_STREAM_END)
		return;

	unsigned int* pixels = (unsigned int*)*(int*)(image + 72);
	int pitch = *(int*)(image + 60);
	memset(pixels, 0, 0x400000);

	unsigned int entryCount = (g_playlistPopulationDecompBuffer[3] << 24) | (g_playlistPopulationDecompBuffer[2] << 16) | (g_playlistPopulationDecompBuffer[1] << 8) | g_playlistPopulationDecompBuffer[0];

	uint8_t* entry = g_playlistPopulationDecompBuffer + 4;

	for (unsigned int i = 0; i < entryCount; i++, entry += 5)
	{
		short lat = (entry[1] << 8) | entry[0];
		short lon = (entry[3] << 8) | entry[2];
		uint8_t weight = entry[4];

		float y = ((lat * 0.01f) + 90.0f) * 5.688889f;
		float x = ((lon * 0.01f) + 180.0f) * 2.8444445f;

		int ix = (int)x & 1023;
		int iy = 1023 - ((int)y & 1023);

		int index = 4 * XGAddress2DTiledOffset(ix, iy, pitch, 4);
		((uint8_t*)pixels)[index] += weight;
	}

	double sum = 0.0;
	int count = 0;

	for (int i = 0; i < 0x100000; i++)
	{
		unsigned int val = ((uint8_t*)pixels)[i * 4];

		if (val)
		{
			sum += val;
			count++;
		}
	}

	if (count < 2)
		count = 2;

	double mean = sum / count;

	double variance = 0.0;

	for (int i = 0; i < 0x100000; i++)
	{
		unsigned int val = ((uint8_t*)pixels)[i * 4];

		if (val)
		{
			double diff = val - mean;
			variance += diff * diff;
		}
	}

	float stddev = sqrtf((float)(variance / (count - 1)));

	for (int i = 0; i < 0x100000; i++)
	{
		uint8_t* px = &((uint8_t*)pixels)[i * 4];

		if (*px)
		{
			float normalized = (*px - mean) / stddev;

			if (normalized > 1.0f)
				normalized = 1.0f;

			if (normalized < -1.0f)
				normalized = -1.0f;

			uint8_t out = (uint8_t)((normalized * 63.0f) + 128.0f);

			px[0] = out;
			px[1] = out;
			px[2] = out;
			px[3] = out;
		}
	}
}

struct dwFileOperationInfo
{
	// stub
};

struct TaskRecord
{
	// stub
};

typedef void(*Com_Printf_t)(int channel, const char* fmt, ...);
Com_Printf_t Com_Printf = Com_Printf_t(0x824BA068);

CDetour LiveStorage_WriteDWUserFile_Detour;
TaskRecord* LiveStorage_WriteDWUserFile(int controllerIndex, dwFileOperationInfo* fileInfo, int a3, int a4, int64_t a5, int64_t a6)
{
	Com_Printf(0, "Tried to write user file\n");
	return 0;
}

void CBitDemon::RegisterHooks()
{
	// Load the globe's heat map
	LiveStorage_FetchPlaylistPopulation_Detour.CreateDetour(0x8275FAA0, LiveStorage_FetchPlaylistPopulation);
	LiveStorage_GetPlaylistPopulationBuffer_Detour.CreateDetour(0x8275FC80, LiveStorage_GetPlaylistPopulationBuffer);
	UI_GeneratePlaylistPopulationTextureInternal_Detour.CreateDetour(0x8253C4F0, UI_GeneratePlaylistPopulationTextureInternal);

	// Write user files to disk
	LiveStorage_WriteDWUserFile_Detour.CreateDetour(0x827555B0, LiveStorage_WriteDWUserFile);
}
