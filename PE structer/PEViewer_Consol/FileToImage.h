#ifndef FILETOIMAGE_H_
#define FILETOIMAGE_H_
#include "PEInject.h"
#include <string.h>

extern unsigned int gDosNtHeaderValue[57];
extern unsigned char* gpSectionTable;
extern unsigned char* gpDosNtHeader;

int fnExtendAndSaveFile(FILE* pTargetFile);

unsigned char* fnCopyEXE(FILE* pTargetFile);
unsigned char* fnExtendEXE(char* pFileBuffer);
unsigned char* fnBackToFile(unsigned char* pImageBuffer);
unsigned int fnGetFileSizeFromImage(unsigned char* pImageBuffer_Base);
#endif