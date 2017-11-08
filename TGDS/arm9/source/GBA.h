// -*- C++ -*-
// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2005 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef VBA_GBA_H
#define VBA_GBA_H

#include "typedefs.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "System.h"
#include "ichflysettings.h"
#include "Port.h"

#define SAVE_GAME_VERSION_1 1
#define SAVE_GAME_VERSION_2 2
#define SAVE_GAME_VERSION_3 3
#define SAVE_GAME_VERSION_4 4
#define SAVE_GAME_VERSION_5 5
#define SAVE_GAME_VERSION_6 6
#define SAVE_GAME_VERSION_7 7
#define SAVE_GAME_VERSION_8 8
#define SAVE_GAME_VERSION_9 9
#define SAVE_GAME_VERSION  SAVE_GAME_VERSION_9

typedef struct {
  u8 *address;
  u32 mask;
} memoryMap;

typedef union {
  struct {
#ifdef WORDS_BIGENDIAN
    u8 B3;
    u8 B2;
    u8 B1;
    u8 B0;
#else
    u8 B0;
    u8 B1;
    u8 B2;
    u8 B3;
#endif
  } B;
  struct {
#ifdef WORDS_BIGENDIAN
    u16 W1;
    u16 W0;
#else
    u16 W0;
    u16 W1;
#endif
  } W;
#ifdef WORDS_BIGENDIAN
  volatile u32 I;
#else
	u32 I;
#endif
} reg_pair;

#define R13_IRQ  18
#define R14_IRQ  19
#define SPSR_IRQ 20
#define R13_USR  26
#define R14_USR  27
#define R13_SVC  28
#define R14_SVC  29
#define SPSR_SVC 30
#define R13_ABT  31
#define R14_ABT  32
#define SPSR_ABT 33
#define R13_UND  34
#define R14_UND  35
#define SPSR_UND 36
#define R8_FIQ   37
#define R9_FIQ   38
#define R10_FIQ  39
#define R11_FIQ  40
#define R12_FIQ  41
#define R13_FIQ  42
#define R14_FIQ  43
#define SPSR_FIQ 44

#include "Cheats.h"
#include "Globals.h"
#include "EEprom.h"
#include "Flash.h"

#define UPDATE_REG(address, value)\
{\
	WRITE16LE(((u16 *)&ioMem[address]),value);\
}

#define CPUReadByteQuick(addr) \
  map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]

#define CPUReadHalfWordQuick(addr) \
  READ16LE(((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

#define CPUReadMemoryQuick(addr) \
  READ32LE(((u32*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))


#define ARM_PREFETCH \
  {\
    cpuPrefetch[0] = CPUReadMemoryQuick(armNextPC);\
    cpuPrefetch[1] = CPUReadMemoryQuick(armNextPC+4);\
  }

#define THUMB_PREFETCH \
  {\
    cpuPrefetch[0] = CPUReadHalfWordQuick(armNextPC);\
    cpuPrefetch[1] = CPUReadHalfWordQuick(armNextPC+2);\
  }

#define ARM_PREFETCH_NEXT \
  cpuPrefetch[1] = CPUReadMemoryQuick(armNextPC+4);

#define THUMB_PREFETCH_NEXT\
  cpuPrefetch[1] = CPUReadHalfWordQuick(armNextPC+2);

#ifdef __GNUC__
#define _stricmp strcasecmp
#endif

#define log(...) printf(__VA_ARGS__)

#endif //VBA_GBA_H

#ifdef __cplusplus
extern "C" {
#endif

extern void emuInstrARM(u32 opcode, s32 *R);
extern void emuInstrTHUMB(u16 opcode, s32 *R);

extern void unknowndebugprint(reg_pair *myregs);
extern void unkommeopcode(u32 opcode, reg_pair *myregs);

extern inline void updateVCsub();
extern inline void updateVC();

extern u8  ichfly_readu8extern(unsigned int pos);
extern u16 ichfly_readu16extern(unsigned int pos);
extern u32 ichfly_readu32extern(unsigned int pos);

extern	void 	doDMAFIFO(u32 s, u32 d, u32 si, u32 di, u32 c, int transfer32);
extern	void 	doDMAslow(u32 s, u32 d, u32 si, u32 di, u32 c, int transfer32);
extern	void	doDMA(u32 s, u32 d, u32 si, u32 di, u32 c, int transfer32);

extern char disbuffer[0x2000];
extern void debugDump();
extern u32 myROM[];

//ichfly i use VisualBoyAdvance instead of normal funktions because i know them
extern FILE * pFile;
extern u8 cpuBitsSet[256];
extern int framenummer;

extern int bg;
extern int emulating;


#ifndef NO_GBA_MAP
extern memoryMap map[256];
#endif

extern reg_pair reg[45];
extern u8 biosProtected[4];

extern bool N_FLAG;
extern bool Z_FLAG;
extern bool C_FLAG;
extern bool V_FLAG;
extern bool armIrqEnable;
extern bool armState;
extern int armMode;
extern void (*cpuSaveGameFunc)(u32,u8);

//#ifdef BKPT_SUPPORT //ichfly
extern u8 freezeWorkRAM[0x40000];
extern u8 freezeInternalRAM[0x8000];
extern u8 freezeVRAM[0x18000];
extern u8 freezeOAM[0x400];
extern u8 freezePRAM[0x400];
extern bool debugger_last;
extern int  oldreg[17];
extern char oldbuffer[10];
//#endif

#ifdef usebuffedVcout
extern u8 VCountgbatods[0x100]; //(LY)      (0..227) + check overflow
extern u8 VCountdstogba[263]; //(LY)      (0..262)
extern u8 VCountdoit[263]; //jump in or out
#endif



//C++ declares
extern bool CPUReadGSASnapshot(const char *);
extern bool CPUWriteGSASnapshot(const char *, const char *, const char *, const char *);
extern bool CPUWriteBatteryFile(const char *);
extern bool CPUReadBatteryFile(const char *);
extern bool CPUExportEepromFile(const char *);
extern bool CPUImportEepromFile(const char *);
extern bool CPUWritePNGFile(const char *);
extern bool CPUWriteBMPFile(const char *);
extern void CPUCleanUp();
extern void CPUUpdateRender();
extern bool CPUReadMemState(char *, int);
extern bool CPUReadState(const char *);
extern bool CPUWriteMemState(char *, int);
extern bool CPUWriteState(const char *);
extern int CPULoadRom(const char *,bool);
extern void doMirroring(bool);
extern void CPUUpdateRegister(u32, u16);
extern void applyTimer ();
//extern void CPUWriteHalfWord(u32, u16);
//extern void CPUWriteByte(u32, u8);
extern void CPUInit(const char *,bool,bool);
extern void CPUReset();
extern void CPULoop(int);
extern void CPUCheckDMA(int,int);
extern bool CPUIsGBAImage(const char *);
extern bool CPUIsZipFile(const char *);
#ifdef PROFILING
#include "prof/prof.h"
extern void cpuProfil(profile_segment *seg);
extern void cpuEnableProfiling(int hz);
#endif

extern void cpu_SetCP15Cnt(u32 v);
extern u32 cpu_GetCP15Cnt();

extern void VblankHandler();
extern void frameasyncsync();
extern void pausemenue();

extern void BIOScall(int op,  s32 *R);

extern s32  __attribute__((section(".dtcm"))) exRegs[]; //automatisierung

extern int SPtoload;
extern int SPtemp;

extern void IntrMain();
extern void testasm(u32* feld);
extern void cpu_SetCP15Cnt(u32 v);
extern u32 cpu_GetCP15Cnt();
extern u32 pu_Enable();
extern int frameskip;
extern int framewtf;


extern void emulateedbiosstart();
extern void HblankHandler(void);
extern void downgreadcpu();
extern void initspeedupfelder();
extern void resettostartup();
extern void IntrMain();
extern void testasm(u32* feld);
extern void cpu_SetCP15Cnt(u32 v);
extern u32 cpu_GetCP15Cnt();

extern bool cpuIsMultiBoot;
extern void getandpatchmap(int offsetgba,int offsetthisfile);
extern void arm7dmareqandcheat();

extern void pu_SetRegion(u32 region, u32 value);
extern void pu_SetDataPermissions(u32 v);
extern void pu_SetCodePermissions(u32 v);
extern void  pu_SetDataCachability(u32 v);
extern void  pu_SetCodeCachability(u32 v);
extern void pu_GetWriteBufferability(u32 v);
extern void gbaExceptionHdl();
extern bool disableMessage;
extern u32 savedsp;
extern u32 savedlr;
//extern void swiHalt(void);


#ifdef __cplusplus
}
#endif

