#ifndef RVATOOFA_H_
#define RVATOOFA_H_
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define _IN_
#define _OUT_

#define BYTE 1
#define WORD 2
#define DWORD 4

#define VIRTUAL_ADDRESS_OFFSET 0x0C
#define POINTER_TO_RAWDATA_OFFSET 0x14

unsigned int RvaToFoa(_IN_ FILE* pTargetFile, _IN_ unsigned int RvaAddress);
#endif // !RVATOOFA_H_