#ifndef PEINFO_H_
#define PEINFO_H_
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include <string.h>

#define BYTE 1
#define WORD 2
#define DWORD 4
#define SECTION_TABLE_SIZE 10
#define _IN_
#define _OUT_

int GetPEinfo(FILE*, bool);
int fnPrintPEinfo();
#endif // !PEINFO_H_