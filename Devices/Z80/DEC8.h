#include "Z80Instruction.h"
#ifndef __Z80_DEC8_H__
#define __Z80_DEC8_H__
namespace Z80 {

class DEC8 :public Z80Instruction
{
public:
	virtual DEC8* Clone() {return new DEC8();}
	virtual DEC8* ClonePlacement(void* buffer) {return new(buffer) DEC8();}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"00***101", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"DEC", target.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		if(target.Decode8BitRegister(data.GetDataSegment(3, 3)))
		{
			//DEC r		00rrr101
			AddExecuteCycleCount(4);
		}
		else
		{
			//DEC (HL)		00110101
			//DEC (IX + d)	11011101 00110101
			//DEC (IY + d)	11111101 00110101
			target.SetMode(EffectiveAddress::MODE_HL_INDIRECT);
			if(GetIndexState() == EffectiveAddress::INDEX_NONE)
			{
				AddExecuteCycleCount(11);
			}
			else
			{
				AddExecuteCycleCount(19);
			}
		}

		AddInstructionSize(GetIndexOffsetSize(target.UsesIndexOffset()));
		AddInstructionSize(target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte result;

		//Perform the operation
		additionalTime += target.Read(cpu, location, op1);
		result = op1 - 1;
		additionalTime += target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagS(result.Negative());
		cpu->SetFlagZ(result.Zero());
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH(!op1.GetBit(3) && result.GetBit(3));
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagPV(op1.Negative() && !result.Negative());
		cpu->SetFlagN(true);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress target;
};

} //Close namespace Z80
#endif