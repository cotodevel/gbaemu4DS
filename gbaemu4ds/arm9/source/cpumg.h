#ifndef __CPU_H__
#define __CPU_H__

#include <nds/jtypes.h>

#ifdef __cplusplus
extern "C"{
#endif

// void cpu_GbaMemPerm();
// void cpu_NdsMemPerm();

// extern void cpu_GbaSetIwram();

void cpu_ArmJump(u32 address, u32 r1);
void cpu_ArmJumpforstackinit(u32 address, u32 r1);
void exInitswisystem(void (*customswiHdl)());
void gbaswieulatedbios();

void gbaMode2();

void gbaInit(bool slow);
void switch_to_unprivileged_mode();
void gbaMode();
void ndsMode();
u32 getExceptionAddress( u32 opcodeAddress, u32 thumbState);
unsigned long ARMShift(unsigned long value,unsigned char shift);

void readbankedextra(u32 CPSR);
u32 cpuGetCPSR();
u32 cpuGetSPSR();
void cpuSetCPSR(u32 CPSR);

u32 readbankedsp(u32 CPSR);
u32 readbankedlr(u32 CPSR);

void cpupausemodeexit();
void cpupausemode();
void debugDump();


void ichflyswiHalt();
void ichflyswiWaitForVBlank();
void ichflyswiIntrWait(u32 i,u32 c);


#ifdef __cplusplus
}
#endif

#endif /*__CPU_H__*/
 
 
