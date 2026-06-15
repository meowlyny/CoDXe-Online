#pragma once

class CDetour
{
public:
	CDetour();
	CDetour(void* place, void* target);
	CDetour(size_t place, void* target);
	~CDetour();

	bool CreateDetour(void* place, void* target);
	bool CreateDetour(size_t place, void* target);
	bool ClearDetour();

	void* GetTrampolineAddress() const
	{
		return m_trampolineAddress;
	}

	static BYTE m_trampolineBuf[200 * 20];
	static SIZE_T m_trampolineSize;

#pragma region Ugly InvokeFunction Variadic Hack
	template<typename Ret>
	Ret InvokeFunction() const
	{
		return reinterpret_cast<Ret(*)()>(this->GetTrampolineAddress())();
	}

	template<typename Ret, typename A1>
	Ret InvokeFunction(A1 a1) const
	{
		return reinterpret_cast<Ret(*)(A1)>(this->GetTrampolineAddress())(a1);
	}

	template<typename Ret, typename A1, typename A2>
	Ret InvokeFunction(A1 a1, A2 a2) const
	{
		return reinterpret_cast<Ret(*)(A1, A2)>(this->GetTrampolineAddress())(a1, a2);
	}

	template<typename Ret, typename A1, typename A2, typename A3>
	Ret InvokeFunction(A1 a1, A2 a2, A3 a3) const
	{
		return reinterpret_cast<Ret(*)(A1, A2, A3)>(this->GetTrampolineAddress())(a1, a2, a3);
	}

	template<typename Ret, typename A1, typename A2, typename A3, typename A4>
	Ret InvokeFunction(A1 a1, A2 a2, A3 a3, A4 a4) const
	{
		return reinterpret_cast<Ret(*)(A1, A2, A3, A4)>(this->GetTrampolineAddress())(a1, a2, a3, a4);
	}

	template<typename Ret, typename A1, typename A2, typename A3, typename A4, typename A5>
	Ret InvokeFunction(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
	{
		return reinterpret_cast<Ret(*)(A1, A2, A3, A4, A5)>(this->GetTrampolineAddress())(a1, a2, a3, a4, a5);
	}

	template<typename Ret, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	Ret InvokeFunction(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
	{
		return reinterpret_cast<Ret(*)(A1, A2, A3, A4, A5, A6)>(this->GetTrampolineAddress())(a1, a2, a3, a4, a5, a6);
	}

	template<typename Ret, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	Ret InvokeFunction(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
	{
		return reinterpret_cast<Ret(*)(A1, A2, A3, A4, A5, A6, A7)>(this->GetTrampolineAddress())(a1, a2, a3, a4, a5, a6, a7);
	}

	template<typename Ret, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	Ret InvokeFunction(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
	{
		return reinterpret_cast<Ret(*)(A1, A2, A3, A4, A5, A6, A7, A8)>(this->GetTrampolineAddress())(a1, a2, a3, a4, a5, a6, a7, a8);
	}

	template<typename Ret, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	Ret InvokeFunction(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
	{
		return reinterpret_cast<Ret(*)(A1, A2, A3, A4, A5, A6, A7, A8, A9)>(this->GetTrampolineAddress())(a1, a2, a3, a4, a5, a6, a7, a8, a9);
	}

	template<typename Ret, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	Ret InvokeFunction(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
	{
		return reinterpret_cast<Ret(*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>(this->GetTrampolineAddress())(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
	}

	// Add more if you have more then 10 params
#pragma endregion

private:
	void* m_place, *m_target;
	unsigned char m_originalInstructions[32];
	size_t m_originalLength;
	void* m_trampolineAddress;

	static size_t CopyInstruction(UINT32* dest, const UINT32* src);
	static size_t RelocateBranch(UINT32* dest, const UINT32* src);

	static size_t WriteFarBranch(void* dest, const void* target, bool linked, bool preserveReg);
	static size_t WriteFarBranchEx(void* dest, const void* target, bool linked, bool preserveReg, UINT32 options, BYTE crBit, BYTE regIndex);
};

extern CDetour detour;
