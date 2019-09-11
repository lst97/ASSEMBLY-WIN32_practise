#ifndef FILETOIMAGE_H_
#define FILETOIMAGE_H_
#include "PEinfo.h"
#include <string.h>

extern unsigned int DosNtHeaderValue[57];
extern unsigned int* pSectionTableValue;
extern unsigned int* pSectionNameBuffer;
extern unsigned char* pSectionTable;
extern unsigned char* pDosNtHeader;

int fnExtendAndSaveFile(FILE* pTargetFile);

unsigned char* fnCopyEXE(FILE* pTargetFile);
unsigned int fnGetFileSize();
unsigned char* fnExtendEXE(char* pFileBuffer);
unsigned char* fnBackToFile(unsigned char* pImageBuffer);
unsigned int fnGetFileSizeFromImage(unsigned char* pImageBuffer_Base);
#endif