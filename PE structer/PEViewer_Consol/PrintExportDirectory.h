#ifndef PRINTEXPORTDIRECTORY_H_
#define PRINTEXPORTDIRECTORY_H_
#include "stdio.h"
#include "stdlib.h"
#include "RvaToFoa.h"

int PrintExportDirectory(FILE* pTargetFile);
unsigned char* fnGetStringBufferOffset(_IN_ unsigned char* pStringBuffer, _IN_ unsigned int offset);
#endif // !PRINTEXPORTDIRECTORY_H_

