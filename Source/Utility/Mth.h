#pragma once

#define INF		FLT_MAX

class CMth
{
public:
	static long lround(double value)
	{
		return value >= 0.0 ? static_cast<long>(value + 0.5) : static_cast<long>(value - 0.5);
	}

	static long lround(float value)
	{
		return lround(static_cast<double>(value));
	}
};

extern CMth math;
