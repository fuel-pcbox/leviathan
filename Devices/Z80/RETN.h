#include "Z80Instruction.h"
#ifndef __Z80_RETN_H__
#define __Z80_RETN_H__
namespace Z80 {

class RETN :public Z80Instruction
{
public:
	virtual RETN* Clone() {return new RETN();}
	virtual RETN* ClonePlacement(void* buffer) {return new(buffer) RETN();}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"01000101", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"RETN", L"");
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetMode(EffectiveAddress::MODE_SP_POSTINC);
		AddExecuteCycleCount(10);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		//Perform the operation
		cpu->SetIFF1(cpu->GetIFF2());
		Z80Word oldPC;
		additionalTime += source.Read(cpu, location + GetInstructionSize(), oldPC);
		cpu->PopCallStack(oldPC.GetData());
		cpu->SetPC(oldPC);

		//Return the execution time
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
};

} //Close namespace Z80
#endif