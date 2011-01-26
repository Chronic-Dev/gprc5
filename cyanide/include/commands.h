/**
  * GreenPois0n Cynanide - commands.h
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef COMMANDS_H
#define COMMANDS_H

#include "device.h"
#include "offsets.h"

#define MAX_COMMANDS 40

#define CMDARG_TYPE_STRING 0
#define CMDARG_TYPE_INTEGER 1

typedef struct CmdArg {
	signed int unk1;
	unsigned int uinteger;
	signed int integer;
	unsigned int type;
	unsigned char* string;
} CmdArg;

typedef int(*CmdFunction)(int argc, CmdArg* argv);

typedef struct CmdInfo {
	char* name;
	CmdFunction handler;
	char* description;
} CmdInfo;

extern unsigned char* gCmdListBegin;
extern unsigned char* gCmdListEnd;

extern int gCmdCount;
extern Bool gCmdHasInit;
extern CmdInfo** gCmdCommands;

struct boot_args {
    unsigned short something; // 0 - 1
    unsigned short epoch; // 2 - must be 2 (6 on iPhone1,2 etc...?)
    unsigned int virtbase; // 4
    unsigned int physbase; // 8
    unsigned int size; // c
    unsigned int pt_paddr; // 10: | 0x18 (eh, but we're 0x4000 off) -> ttbr1
    unsigned int v_baseAddr; // 14 (-> PE_state+4 - v_baseAddr) 5f700000
    unsigned int v_display; // 18 (-> PE_state+0x18 - v_display) 1
    unsigned int v_rowBytes;  // 1c (-> PE_state+8 - v_rowBytes) 2560
    unsigned int v_width; // 20 (-> PE_state+0xc - v_width) 640
    unsigned int v_height; // 24 (-> PE_state+0x10 - v_height) 960
    unsigned int v_depth; // 28 (-> PE_state+0x14 - v_depth) 65568?
    unsigned int unk2c; // 2c
    char     *dt_vaddr; // 30 (-> PE_state+0x6c)
    unsigned int dt_size; // 34
    char     cmdline[255]; // 38
};

extern int(*jump_to)(int flags, void* addr, struct boot_args* bootargs);
extern int(*load_ramdisk)(int argc);

int cmd_init();
void cmd_add(char* name, CmdFunction handler, char* description);

int cmd_help(int argc, CmdArg* argv);
int cmd_echo(int argc, CmdArg* argv);
int cmd_hexdump(int argc, CmdArg* argv);
int cmd_jump(int argc, CmdArg* argv);
int cmd_mw(int argc, CmdArg* argv);
int cmd_md(int argc, CmdArg* argv);
int cmd_call(int argc, CmdArg* argv);
int cmd_fsboot(int argc, CmdArg* argv);
int cmd_rdboot(int argc, CmdArg* argv);
int cmd_ramdisk(int argc, CmdArg* argv);
int cmd_test(int argc, CmdArg* argv);

#endif
