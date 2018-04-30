/*---------------------------------------------------------------------------------

	Basic template code for starting a GBA app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include <filesystem.h>

#include "../../common/cpuglobal.h"
#include "../../common/gba_ipc.h"
#include "interrupts/fifo_handler.h"

#include "GBA.h"
#include "Sound.h"
#include "Util.h"
#include "getopt.h"
#include "System.h"
#include <fat.h>
#include <dirent.h>
#include "cpumg.h"
#include "GBAinline.h"
#include "bios.h"
#include "mydebuger.h"
#include "file_browse.h"
#include "arm7sound.h"
#include "main.h"
#include "memory_handler_gba/mem_handler.h"
#include "ipc/touch_ipc.h"

#include <stdio.h>
#include <stdlib.h>
#include <nds/memory.h>//#include <memory.h> ichfly
#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/bios.h>
#include <nds/system.h>
#include <nds/arm9/math.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/sassert.h>
#include <stdarg.h>
#include <string.h>
#include "libnds_extends/interrupts.h"

//dmabuffer (old gbaemu4ds way)
//volatile u8 arm7exchangefild[0x100];

#define INT_TABLE_SECTION __attribute__((section(".dtcm")))

extern struct IntTable irqTable[MAX_INTERRUPTS] INT_TABLE_SECTION;

#include <nds/arm9/dldi.h>

// The only built in driver
extern DLDI_INTERFACE _io_dldi_stub;

//#define loaddirect



#define GBA_EWRAM ((void*)(0x02000000))

#include <nds/disc_io.h>
#include <dirent.h>

   #define DEFAULT_CACHE_PAGES 16
   #define DEFAULT_SECTORS_PAGE 8

#define public


char biosPath[MAXPATHLEN * 2];
char patchPath[MAXPATHLEN * 2];
char savePath[MAXPATHLEN * 2];
char szFile[MAXPATHLEN * 2];

char* savetypeschar[7] =
	{"SaveTypeAutomatic","SaveTypeEeprom","SaveTypeSram","SaveTypeFlash64KB","SaveTypeEepromSensor","SaveTypeNone","SaveTypeFlash128KB"};

char* memoryWaitrealram[8] =
  { "10 and 6","8 and 6","6 and 6","18 and 6","10 and 4","8 and 4","6 and 4","18 and 4" };


/*
#ifdef anyarmcom
u32 amr7sendcom = 0;
u32 amr7senddma1 = 0;
u32 amr7senddma2 = 0;
u32 amr7recmuell = 0;
u32 amr7directrec = 0;
u32 amr7indirectrec = 0;
u32 amr7fehlerfeld[10];
#endif
*/

//---------------------------------------------------------------------------------
int main( int argc, char **argv) {

  biosPath[0] = 0;
  savePath[0] = 0;
  patchPath[0] = 0;




//---------------------------------------------------------------------------------
	//set the mode for 2 text layers and two extended background layers
	//videoSetMode(MODE_5_2D); 
  videoSetModeSub(MODE_5_2D);

	
	//defaultExceptionHandler();	//for debug befor gbainit

//ori
/*
	//set the first two banks as background memory and the third as sub background memory
	//D is not used..if you need a bigger background then you will need to map
	//more vram banks consecutivly (VRAM A-D are all 0x20000 bytes in size)
	vramSetPrimaryBanks(
	VRAM_A_MAIN_BG_0x06000000,//for gba 
	VRAM_B_LCD, 
	VRAM_C_SUB_BG_0x06200000,
	VRAM_D_MAIN_BG_0x06020000
	); //needed for main emulator
	
	vramSetBanks_EFG(
	VRAM_E_MAIN_SPRITE//for gba sprite,
	VRAM_F_LCD,
	VRAM_G_LCD
	);
	vramSetBankH(VRAM_H_SUB_BG); //only sub //for prints to lowern screen 
	vramSetBankI(VRAM_I_SUB_BG_0x06208000); //only sub
*/

//coto: fixed mode1 (partially)
vramSetPrimaryBanks(	
VRAM_A_MAIN_BG_0x06000000,      //Mode0 Tile/Map mode
VRAM_B_MAIN_BG_0x06020000,      //Mode 1/2/3/4 special bitmap/rotscale modes (engine A bg0,1,2,3 needs them)
VRAM_C_LCD,
VRAM_D_LCD
); //needed for main emulator


vramSetBanks_EFG(
VRAM_E_MAIN_SPRITE,        //GBA -> NDS Sprites (same PPU format!)
VRAM_F_LCD,                
VRAM_G_LCD
);
vramSetBankH(VRAM_H_SUB_BG); //only sub //for prints to lowern screan 
vramSetBankI(VRAM_I_SUB_BG_0x06208000); //only sub

#ifdef advanced_irq_check
	irqSet(IRQ_HBLANK,HblankHandler); //todo async
#ifdef forceHBlankirqs
	irqEnable(IRQ_HBLANK);
#endif
#endif
	
	//fifo init setup ORI
	//__irqSet(IRQ_FIFO_NOT_EMPTY,HandleFifo,irqTable); //todo async
	//irqEnable(IRQ_FIFO_NOT_EMPTY);
	
	__irqSet(IRQ_FIFO_NOT_EMPTY,HandleFifo,irqTable); //todo async
	irqEnable(IRQ_FIFO_NOT_EMPTY);

	 // REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR | IPC_FIFO_RECV_IRQ;
    REG_IPC_FIFO_CR |= (1<<2);  //send empty irq
    REG_IPC_FIFO_CR |= (1<<10); //recv empty irq
    
    *(u16*)0x04000184 = *(u16*)0x04000184 | (1<<15); //enable fifo send recv
    
	REG_IF = ~0;
	
	//bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	consoleDemoInit();

	REG_POWERCNT &= ~((POWER_3D_CORE | POWER_MATRIX) & 0xFFFF);//powerOff(POWER_3D_CORE | POWER_MATRIX); //3D use power so that is not needed

	//consoleDemoInitsubsc();
	//iprintf("gbaemu4DS for r4i gold (3DS) (r4ids.cn) by ichfly\nBuildday" __DATE__);
	//consoleDemoInit();
	//soundEnable(); //sound finaly
	//fifoSetDatamsgHandler(FIFO_USER_02, arm7debugMsgHandler, 0);




	DISPCNT  = 0x0080;

//rootenabelde[2] = fatMountSimple  ("sd", &__io_dsisd); //DSi//sems to be inited by fatInitDefault
 //fatInitDefault();
 //nitroFSInit();
bool temptest = true;
if(argv[5][0] == '1')
{
	lcdSwap();
}

if(!(_io_dldi_stub.friendlyName[0] == 0x52 && _io_dldi_stub.friendlyName[5] == 0x4E) && temptest)
{
		iprintf("gbaemu DS for r4i gold (3DS) (r4ids.cn) by ichfly\n");
		iprintf("Warning: you try to run gbaemu DS on %s gbaemu may not work\n press A to continue and ignore this",_io_dldi_stub.friendlyName);
	while(1) {
		scanKeys();
		if (keysDown()&KEY_A) break;
		if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
		while(!(REG_DISPSTAT & DISP_IN_VBLANK));
	}
}
else
{	
	//antysoftreset
	/*if(*(u32*)((u8*)(IntrMain) + 0x18) == 0xE51FF004)
	{
		int oldIME = enterCriticalSection();
		*(u32*)((u8*)(IntrMain) + 0x18) = 0xE14F0000;
		*(u32*)((u8*)(IntrMain) + 0x1C) = 0xE92D5002;
		leaveCriticalSection(oldIME);


	  //send seed patch
	  //fifoSendValue32(FIFO_USER_01,0x40000000);
	  //fifoSendValue32(FIFO_USER_01,0xAF8CA2B8);


	}*/
}

//coto:
/*
//dma shared buffer (old)
#ifdef arm9advsound
REG_IPC_FIFO_TX = 0x1FFFFFFA; //load buffer
REG_IPC_FIFO_TX =  (u32)&arm7exchangefild[0]; //buffer for arm7
#endif
*/

/*REG_IPC_FIFO_TX = 0;
while(true)
{
	int i = REG_IPC_FIFO_RX;
	iprintf("%08X\r\n",i);
	REG_IPC_FIFO_TX = i;

}*/

/*	iprintf("\n%x %x %x",getHeapStart(),getHeapEnd(),getHeapLimit());
malloc(0x4000);
iprintf("\n%x %x %x",getHeapStart(),getHeapEnd(),getHeapLimit());
	while(1);*/ //test getHeapEnd() is the needed thing

	iprintf("Init Fat...");
    



    if(fatInitDefault()){
        iprintf("OK\n");
    }else{
        iprintf("failed\n");
        int i = 0;
		while(i< 300)
		{
			//swiWaitForVBlank();
			if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
			while(!(REG_DISPSTAT & DISP_IN_VBLANK));
			i++;
		}
    }

	//irqInit();
	

//main men� ende aber bleibe im while

	

	//dirfolder("nitro:/");
	
	
	bool nichtausgewauhlt = true;
	

	
	iprintf("\x1b[2J");
//main men�

//while(1)iprintf("o");


strcpy(szFile,argv[1]);
strcpy(savePath,argv[2]);
strcpy(biosPath,argv[3]);
strcpy(patchPath,argv[4]);
if(argv[11][0] == '1')cpuIsMultiBoot = true;
else cpuIsMultiBoot = false;
int myflashsize = 0x10000;

u32 manual_save_type = (u32)strtol(argv[6],NULL,16);

//ori
/*
if(save_type == 6)
{
	myflashsize = 0x20000;
	cpuSaveType = 3;
}
else
{
	cpuSaveType = save_type;
}
*/


frameskip = (u32)strtol(argv[7],NULL,16);
int syncline =(u32)strtol(argv[9],NULL,16);
bool slow;
if(argv[10][0] == '1')slow = true;
else slow = false;
if(argv[8][0] == '1')
{
	REG_IPC_FIFO_TX = 0x1FFFFFFC; //send cmd
	REG_IPC_FIFO_TX = 0;
}
	



	//sprintf(szFile,"%s","nitro:/puzzle.gba"); //ichfly test
	
	/*scanKeys();

	while(keysDownRepeat()&KEY_A)
	{
		scanKeys();
		swiWaitForVBlank();
	}*/

	bool extraram =false; 
	//if(!REG_DSIMODE) extraram = ram_init(DETECT_RAM); 
	//extraram = true; //testtest
#ifdef usebuffedVcout
initspeedupfelder();
#endif
	//iprintf("\x1b[2J");





  iprintf("\x1b[2J");

  parseDebug = true;

  //if(argc == 2) {
  //iprintf("%s",szFile);
  //while(1);
    bool failed = false;
 
	iprintf("CPULoadRom...");

      failed = !CPULoadRom(szFile,extraram);
	  
	  if(failed)
	  {
		printf("failed");
		while(1);
	  }
	  iprintf("OK\n");
	  
	  
if(save_decider()==0){
	if(manual_save_type == 6)
	{
		myflashsize = 0x20000;
		cpuSaveType = 3;
	}
	else
	{
		cpuSaveType = manual_save_type;
	}
}

		//iprintf("Hello World2!");
		iprintf("CPUInit\n");
		CPUInit(biosPath, useBios,extraram);

	  iprintf("CPUReset\n");
      CPUReset();
	
	/*
	  if(savePath[0] != 0)
	  {
		  iprintf("CPUReadBatteryFile...");
		  if(CPUReadBatteryFile(savePath))
		  {
			iprintf("OK\n");
		  }
		  else
		  {
			  iprintf("failed\n");
				int i = 0;
				while(i< 300)
				{
					//swiWaitForVBlank();
					if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
					while(!(REG_DISPSTAT & DISP_IN_VBLANK));
					i++;
				}
		  }
	  }
	*/
	
	int filepathlen = strlen(szFile);
	char  fn_noext[filepathlen] = {0};
	memcpy(fn_noext,szFile,filepathlen-3);

	//detect savefile (filename.sav)
	sprintf(fn_noext,"%ssav",fn_noext);
	FILE * frh = fopen(fn_noext,"r");

	//if(frh)
	//    iprintf("current save path: %s DO exists",fn_noext);
	//else
	//    iprintf("current save path: %s DONT exists",fn_noext);  
	//while(1);

	//coto: added create new savefile
	if(!frh){
		iprintf("no savefile found, creating new one... \n");
		//append "sav"
		
		//void * memcpy ( void * destination, const void * source, size_t num );
		
		//char * strcat ( char * destination, const char * source );
		
		savePath[0] = 0;
		strcpy ((char *)savePath, (const char *)fn_noext);
		CPUWriteBatteryFile(savePath);
		
		//void * memset ( void * ptr, int value, size_t num );
	}

	else
	{
		strcpy ((char *)savePath, (const char *)fn_noext);
		if(CPUReadBatteryFile(savePath))
		{
			if(cpuSaveType == 0)iprintf("SaveReadOK![AUTO]\n");
			if(cpuSaveType == 1)iprintf("SaveReadOK![EEPROM]\n");
			if(cpuSaveType == 2)iprintf("SaveReadOK![SRAM]\n");
			if(cpuSaveType == 3)iprintf("SaveReadOK![FLASHROM]\n");
			if(cpuSaveType == 4)iprintf("SaveReadOK![EEPROM+SENSOR]\n");
			if(cpuSaveType == 5)iprintf("SaveReadOK![NONE]\n");			
		}
		else
		{
			iprintf("failed reading: %s\n",savePath);
			while(1);
		}
		fclose(frh);
	}
	
	iprintf("BIOS_RegisterRamReset\n");

	cpu_SetCP15Cnt(cpu_GetCP15Cnt() & ~0x1); //disable pu to write to the internalRAM

	BIOS_RegisterRamReset(0xFF);

	pu_Enable();
	
	//memcopy((void*)0x2000000,(void*)rom, 0x40000);
	
	iprintf("dmaCopy\n");

/*REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x7654321;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/

	dmaCopy( (void*)rom,(void*)0x2000000, 0x40000);
	iprintf("arm7init\n");


	VblankHandler();
	REG_IPC_FIFO_TX = 0x1FFFFFFF; //cmd
	REG_IPC_FIFO_TX = syncline;
	while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))u32 src = REG_IPC_FIFO_RX;
	iprintf("irqinit\n");

	anytimejmpfilter = 0;
	
	//anytimejmp = (VoidFn)0x3007FFC; //bios import
/*
REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x1234567;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
	iprintf("emulateedbiosstart\n");

	emulateedbiosstart();
/*REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x1111111;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/

    iprintf("ndsMode\n");

	ndsMode();
/*REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x2222222;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
    
	SendArm7Command(0xc1710001,0x0,0x0,0x0);//fifotest
	
	iprintf("gbaInit\n");


#ifdef capture_and_pars
	videoBgDisableSub(0);
	vramSetBankH(VRAM_H_LCD); //only sub
	vramSetBankI(VRAM_I_LCD); //only sub
	 int iback = bgInitSub(3, BgType_ExRotation, BgSize_B16_256x256, 0,0);

	//bgSetScale(3,0x111,0x133);
	//bgSetRotateScale(iback,0,0x111,0x133);
	//bgSetRotateScale(iback,0,0x0F0,0x0D5);
	bgSetRotateScale(iback,0,0x0F0,0x0D6);
	bgUpdate();
#endif

	gbaInit(slow);


	//iprintf("\n\r%08X",CPUReadMemoryreal(0x08400000));
	//while(1);
/*REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x3333333;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
	//iprintf("irqSet\n");

	//irqSet(IRQ_VBLANK, VblankHandler);

	//irqEnable(IRQ_VBLANK);
/*REG_IPC_FIFO_TX = 0; //test backcall
REG_IPC_FIFO_TX = 0x4444444;
				if((REG_DISPSTAT & DISP_IN_VBLANK)) while((REG_DISPSTAT & DISP_IN_VBLANK)); //workaround
				while(!(REG_DISPSTAT & DISP_IN_VBLANK));*/
#ifndef capture_and_pars
	iprintf("gbaMode2\n");
#endif
	REG_IME = IME_ENABLE;
	gbaMode2();
#ifndef capture_and_pars
	iprintf("jump to (%08X) : RTC: %d:%d:%d \n\r",(unsigned int)rom,(unsigned int)gba_get_hourrtc(),(unsigned int)gba_get_minrtc(),(unsigned int)gba_get_secrtc());
#endif

	//iprintf("\x1b[2J"); //reset (not working huh)
	//show_mem();
	
	touchPosition touchpos;
	touchReadXY_gbaemu4ds(&touchpos);
	
	cpu_ArmJumpforstackinit((u32)rom, 0);
	
	
	
  
  while(true);

		 
  return 0;

}
//a