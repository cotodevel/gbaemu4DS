/*-----------------------------------------------------------------
 Copyright (C) 2005 - 2010
	Michael "Chishm" Chisholm
	Dave "WinterMute" Murphy

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

------------------------------------------------------------------*/

#ifndef FILE_BROWSE_H
#define FILE_BROWSE_H


#define MAXPATHLEN 255
#include <nds.h>

typedef struct
{
	u32 entryPoint;
	u8 logo[156];
	char title[0xC];
	char gamecode[0x4];
	char makercode[0x2];
	u8 is96h;
	u8 unitcode;
	u8 devicecode;
	u8 unused[7];
	u8 version;
	u8 complement;
	u16 res;
	u8 somedata[100000];
} __attribute__ ((__packed__)) gbaHeader_t;

typedef struct
{
	u32 Version;
	u32 listentr;
} __attribute__ ((__packed__)) patch_t;

typedef struct
{
	u32 gamecode;
	u8 homebrew;
	u64 crc;
	char patchPath[MAXPATHLEN * 2];
	u8 swaplcd;
	u8 savfetype;
	u8 frameskip;
	u8 frameskipauto;
	u16 frameline;
	u8 fastpu;
	u8 mb;
	u8 loadertype;
} __attribute__ ((__packed__)) patch2_t;


#endif //FILE_BROWSE_H

#ifdef __cplusplus
	#include <string>
	#include <vector>

	void browseForFile (const std::vector<std::string> extensionList);

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern char biosPath[MAXPATHLEN * 2];

extern char patchPath[MAXPATHLEN * 2];

extern char savePath[MAXPATHLEN * 2];

extern char szFile[MAXPATHLEN * 2];

extern char temppath[MAXPATHLEN * 2];

extern bool cpuIsMultiBoot;

#ifdef __cplusplus
}
#endif
