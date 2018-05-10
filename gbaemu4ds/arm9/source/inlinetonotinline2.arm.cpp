#include "GBA.h"
#include "GBAinline.h"
#include "fatfileextract.h"
#include "Util.h"


u32 CPUReadMemorypu(u32 address)
{
	return CPUReadMemoryrealpu(address);
}


u32 CPUReadHalfWordpu(u32 address)
{
	return CPUReadHalfWordrealpu(address);
}

u8 CPUReadBytepu(u32 address)
{
	return CPUReadByterealpu(address);
}


void CPUWriteMemorypuextern(u32 address, u32 value)
{
	CPUWriteMemorypu(address,value);
}


void CPUWriteHalfWordpuextern(u32 address, u16 value)
{
	CPUWriteHalfWordpu(address,value);
}

void CPUWriteBytepuextern(u32 address, u8 b)
{
	CPUWriteBytepu(address,b);
}


s16 CPUReadHalfWordrealpuSignedoutline(u32 address)
{
	return (s16)CPUReadHalfWordrealpuSigned(address);
}

s8 CPUReadByteSignedpu(u32 address)
{
	return (s8)CPUReadByterealpu(address);
}