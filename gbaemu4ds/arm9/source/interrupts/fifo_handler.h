
#ifndef __fifo_handler_h__
#define __fifo_handler_h__

#include <nds.h>
#include <nds/ndstypes.h>

#ifdef __cplusplus
extern "C"{
#endif

extern void HandleFifo();
extern void arm7dmareqandcheat();

extern u32 arm7amr9buffer;
extern int SPtoload;
extern int SPtemp;

#ifdef __cplusplus
}
#endif

#endif