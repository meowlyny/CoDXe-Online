#pragma once

class CString
{
public:
	const char* Format(const char* fmt, ...)
	{
#define VA_BUFFER_COUNT 4
#define VA_BUFFER_SIZE 32768
		static char buffers[VA_BUFFER_COUNT][VA_BUFFER_SIZE];
		static int currentBuffer = 0;

		char* dest = buffers[currentBuffer];
		currentBuffer = (currentBuffer + 1) % VA_BUFFER_COUNT;

		va_list args;
		va_start(args, fmt);
		int len = vsnprintf(dest, VA_BUFFER_SIZE, fmt, args);
		va_end(args);

		dest[VA_BUFFER_SIZE - 1] = '\0'; // Ensure null-termination

		return dest;
	}

	template<typename T>
	static std::string ToString(T value)
	{
		std::ostringstream oss;
		oss << value;
		return oss.str();
	}
};

extern CString string;
