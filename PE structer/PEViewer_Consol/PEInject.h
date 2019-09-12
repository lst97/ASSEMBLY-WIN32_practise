#ifndef PEINJECT_H_
#define PEINJECT_H_
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define BYTE 1
#define WORD 2
#define DWORD 4

#define _IN_
#define _OUT_

#define MESSAGEBOX_VADDRESS 0x76430380

#define INSTRUCTION_CALL_SIZE 0x5
#define INSTRUCTION_PUSH_SIZE WORD
#define INSTRUCTION_JMP_SIZE 0x5

#define NT_HEADER_SIZE 0x4 + 0x14 + 0x0E0
#define SECTIONTABLE_SIZE 0x28

// OFFSET
#define MISC_OFFSET BYTE * 8
#define IMAGE_BASE_OFFSET 0x1C
#define SIZEOF_OPTION_HEADER 0x0E0
#define ADDRESS_OF_ENTRYPOINT_OFFSET 0x10
#define NUMBER_OF_SECTION_OFFSET DWORD + WORD
#define VIRTUALADDRESS_OFFSET BYTE * 8 + DWORD
#define SIZEOF_RAWDATA_OFFSET BYTE * 8 + DWORD + DWORD
#define POINTER_TO_RAWDATA_OFFSET BYTE * 8 + DWORD + DWORD + DWORD


int fnPEInject(_IN_ FILE* pTargetFile);
static unsigned int fnGetFileSize(_IN_ FILE* pTargetFile);

#endif // !PEINJECT_H_