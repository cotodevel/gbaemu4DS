
#include "fatfile.h"

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#include "cache.h"
#include "file_allocation_table.h"
#include "bit_ops.h"
#include "filetime.h"
#include "lock.h"

#include "ichflysettings.h"

#include "System.h"
#include "Globals.h"

#ifdef __cplusplus
extern "C"{
#endif

extern u8 ichfly_readu8(unsigned int pos);
extern u16 ichfly_readu16(unsigned int pos);
extern u32 ichfly_readu32(unsigned int pos);
extern void ichfly_readdma_rom(u32 pos,u8 *ptr,u32 c,int readal);

#ifdef __cplusplus
}
#endif