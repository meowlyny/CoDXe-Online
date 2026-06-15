#pragma once

class CCompression
{
public:
	static bool Decompress(
		const uint8_t* compressed,
		size_t compressedLen,
		std::vector<uint8_t>& out,
		int windowBits = MAX_WBITS);
};

extern CCompression compression;
