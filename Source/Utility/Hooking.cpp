#include "Hooking.h"
#include "PPC.h"

// This is for Xenia
#pragma section(".text")
__declspec(allocate(".text")) BYTE CDetour::m_trampolineBuf[200 * 20];
SIZE_T CDetour::m_trampolineSize = 0;

CDetour detour;

CDetour::CDetour()
	: m_place(nullptr)
	, m_target(nullptr)
	, m_originalLength(0)
	, m_trampolineAddress(nullptr)
{
}

CDetour::CDetour(void* place, void* target)
	: m_place(nullptr)
	, m_target(nullptr)
	, m_originalLength(0)
	, m_trampolineAddress(nullptr)
{
	this->CreateDetour(place, target);
}

CDetour::CDetour(size_t place, void* target)
	: m_place(nullptr)
	, m_target(nullptr)
	, m_originalLength(0)
	, m_trampolineAddress(nullptr)
{
	this->CreateDetour(reinterpret_cast<void*>(place), target);
}

CDetour::~CDetour()
{
	this->ClearDetour();
}

bool CDetour::CreateDetour(void* place, void* target)
{
	if (this->m_originalLength != 0)
		return false;

	this->m_place = place;
	this->m_target = target;

	size_t hookSize = WriteFarBranch(nullptr, this->m_target, false, false);
	memcpy(this->m_originalInstructions, this->m_place, hookSize);

	this->m_originalLength = hookSize;
	this->m_trampolineAddress = &this->m_trampolineBuf[this->m_trampolineSize];

	for (auto i = 0u; i < hookSize / 4; ++i)
	{
		UINT32* instrPtr = (UINT32*)((UINT32)this->m_place + (i * 4));
		this->m_trampolineSize += CopyInstruction((UINT32*)&this->m_trampolineBuf[this->m_trampolineSize], instrPtr);
	}

	void* afterBranchAddr = (void*)((UINT32)this->m_place + hookSize);
	this->m_trampolineSize += WriteFarBranch(&this->m_trampolineBuf[this->m_trampolineSize], afterBranchAddr, false, true);

	WriteFarBranch(this->m_place, this->m_target, false, false);
	return true;
}

bool CDetour::CreateDetour(const size_t place, void* target)
{
	return this->CreateDetour(reinterpret_cast<void*>(place), target);
}

bool CDetour::ClearDetour()
{
	if (this->m_place && this->m_originalLength != 0)
	{
		memcpy(this->m_place, this->m_originalInstructions, this->m_originalLength);
		this->m_originalLength = 0;
		this->m_place = nullptr;

		return true;
	}

	return false;
}

size_t CDetour::CopyInstruction(UINT32* dest, const UINT32* src)
{
	UINT32 instr = *src;

	switch (instr & PPC_OPCODE_MASK)
	{
	case PPC_OPCODE_B:
	case PPC_OPCODE_BC:
		return RelocateBranch(dest, src);

	default:
		*dest = instr;
		return 4;
	}
}

size_t CDetour::RelocateBranch(UINT32* dest, const UINT32* src)
{
	UINT32 instr = *src;
	UINT32 instrAddr = (UINT32)src;

	if (instr & PPC_BRANCH_ABSOLUTE)
	{
		*dest = instr;
		return 4;
	}

	INT32 offsetBitSize = 0;
	INT32 offsetBitBase = 0;

	UINT32 options = 0;
	BYTE crBit = 0;

	switch (instr & PPC_OPCODE_MASK)
	{
	case PPC_OPCODE_B:
		offsetBitSize = 24;
		offsetBitBase = 2;
		options = PPC_BRANCH_OPTIONS_ALWAYS;
		crBit = 0;
		break;

	case PPC_OPCODE_BC:
		offsetBitSize = 14;
		offsetBitBase = 2;
		options = (instr >> PPC_BIT32(10)) & MASK_N_BITS(5);
		crBit = (instr >> PPC_BIT32(15)) & MASK_N_BITS(5);
		break;
	}

	INT32 offset = instr & (MASK_N_BITS(offsetBitSize) << offsetBitBase);
	if (offset >> ((offsetBitSize + offsetBitBase) - 1))
		offset |= ~MASK_N_BITS(offsetBitSize + offsetBitBase);

	void* addr = (void*)((INT32)instrAddr + offset);

	return WriteFarBranchEx(
		dest,
		addr,
		(instr & PPC_BRANCH_LINKED) != 0,
		true,
		options,
		crBit,
		11);
}

size_t CDetour::WriteFarBranch(
	void* dest,
	const void* target,
	bool linked,
	bool preserveReg)
{
	return WriteFarBranchEx(
		dest,
		target,
		linked,
		preserveReg,
		PPC_BRANCH_OPTIONS_ALWAYS,
		0,
		11);
}

size_t CDetour::WriteFarBranchEx(
	void* dest,
	const void* target,
	bool linked,
	bool preserveReg,
	UINT32 options,
	BYTE crBit,
	BYTE regIndex)
{
	UINT32 farAsm[] = {
		PPC_LIS(regIndex, PPC_HI((UINT32)target)),
		PPC_ORI(regIndex, regIndex, PPC_LO((UINT32)target)),
		PPC_MTCTR(regIndex),
		PPC_BCCTR(options, crBit, linked)
	};

	UINT32 farAsmPreserve[] = {
		PPC_STD(regIndex, -0x30, 1),
		PPC_LIS(regIndex, PPC_HI((UINT32)target)),
		PPC_ORI(regIndex, regIndex, PPC_LO((UINT32)target)),
		PPC_MTCTR(regIndex),
		PPC_LD(regIndex, -0x30, 1),
		PPC_BCCTR(options, crBit, linked)
	};

	const UINT32* asmBlock = preserveReg ? farAsmPreserve : farAsm;
	size_t asmSize = preserveReg ? sizeof(farAsmPreserve) : sizeof(farAsm);

	if (dest)
		memcpy(dest, asmBlock, asmSize);

	return asmSize;
}
