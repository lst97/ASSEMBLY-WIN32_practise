#ifndef ADDSECTIONTABLE_H_
#define ADDSECTIONTABLE_H_
#include "stdio.h"
#include "stdlib.h"
#include "PEInject.h"
#include "stdbool.h"

extern unsigned int gNtHeaderOffset;
extern unsigned int gSectionTableOffset;
extern unsigned int gNumberOfSection;

// Set gobal for latter use
extern unsigned int gVirtualAddress;
extern unsigned int gPointerToRawData;
extern unsigned int gMisc;
extern unsigned int gImageBase;
extern unsigned int gSizeOfRawData;

int fnAddSectionTable(_IN_ FILE* pTargetFile);
bool fnIsInjectTableOversize(_IN_ unsigned char* pNewSectionTable);
unsigned int fnGetNewSectionTableAddress(_IN_ unsigned char* pSectionTable, _IN_ unsigned char* pFileBuffer, _OUT_ unsigned char** pNewSectionTable);

void fnGetLastSectionTableValue(_IN_ unsigned char* pSectionTable, _OUT_ unsigned int* pMisc, unsigned int* pVirtualAddress, unsigned int* pSizeOfRawData, unsigned int* pPointerToRawData);


#endif // !ADDSECTIONTABLE_H_