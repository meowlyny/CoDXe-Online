#pragma once

class CEndianness
{
public:
	static bool IsBigEndian()
	{
		const unsigned short value = 0x0102;
		return (*reinterpret_cast<const unsigned char*>(&value)) == 0x01;
	}

	template<typename T>
	static T ByteSwap(T value)
	{
		unsigned char* bytes = reinterpret_cast<unsigned char*>(&value);
		std::reverse(bytes, bytes + sizeof(T));
		return value;
	}
};

extern CEndianness endianness;
