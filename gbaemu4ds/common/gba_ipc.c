#include <nds.h>
#include "gba_ipc.h"

#ifdef ARM7
void SendArm9Command(u32 command1, u32 command2, u32 command3, u32 command4){
    REG_IPC_FIFO_TX = (u32)command1;
    REG_IPC_FIFO_TX = (u32)command2;
    REG_IPC_FIFO_TX = (u32)command3;
    REG_IPC_FIFO_TX = (u32)command4;
}
#endif


#ifdef ARM9
__attribute__((section(".itcm")))
void SendArm7Command(u32 command1, u32 command2, u32 command3, u32 command4){
    REG_IPC_FIFO_TX = (u32)command1;
    REG_IPC_FIFO_TX = (u32)command2;
    REG_IPC_FIFO_TX = (u32)command3;
    REG_IPC_FIFO_TX = (u32)command4;
}
#endif


//gbaemu4ds clock opcodes
u8 gba_get_yearbytertc(){
	return (u8)GetsIPCSharedGBA()->clockdata[0];
}

u8 gba_get_monthrtc(){
	return (u8)GetsIPCSharedGBA()->clockdata[1];
}

u8 gba_get_dayrtc(){
	return (u8)GetsIPCSharedGBA()->clockdata[2];
}

u8 gba_get_dayofweekrtc(){
	return (u8)GetsIPCSharedGBA()->clockdata[3];
}


u8 gba_get_hourrtc(){
	return (u8)GetsIPCSharedGBA()->clockdata[4];
}

u8 gba_get_minrtc(){
	return (u8)GetsIPCSharedGBA()->clockdata[5];
}

u8 gba_get_secrtc(){
	return (u8)GetsIPCSharedGBA()->clockdata[6];
}


//coto: new GBA SWI sleepmode
#ifdef ARM9
__attribute__((section(".itcm")))
void enterGBASleepMode(){
	Setarm7Sleep(true);
	SendArm7Command(FIFO_SWI_SLEEPMODE_PHASE1,0x0,0x0,0x0);
}
#endif

//Shared Work     027FF000h 4KB    -     -    -    R/W
//Hardware IPC struct packed 
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
struct sIPCSharedGBA* GetsIPCSharedGBA(){
	struct sIPCSharedGBA* sIPCSharedGBAInst = (__attribute__((aligned (4))) struct sIPCSharedGBA*)(0x027FF000);
	return sIPCSharedGBAInst;
}


void Setarm7Sleep(bool value){
	GetsIPCSharedGBA()->arm7asleep = value;
}

bool Getarm7Sleep(){
	return (bool)GetsIPCSharedGBA()->arm7asleep;
}