#include "PEInject.h"

// virtualAddress, Misc, PointerToRawData, sizeOfRawData
unsigned int gNtHeaderOffset = 0;
unsigned int gSectionTableOffset = 0;
unsigned int gNumberOfSection = 0;

// Set goable for latter use
unsigned int gVirtualAddress = 0;
unsigned int gPointerToRawData = 0;
unsigned int gMisc = 0;
unsigned int gImageBase = 0;
unsigned int gAddressOfEntryPoint = 0;

int fnPEInject(_IN_ FILE* pTargetFile) {
	unsigned int fileSize = 0;
	unsigned char* pFileBuffer = NULL;
	unsigned char* pFileBufferBase = NULL;

	fileSize = fnGetFileSize(pTargetFile);

	pFileBuffer = (unsigned char*)malloc(fileSize);
	if (pFileBuffer == NULL) {
		printf("Fail to allocating heap memory");
		getchar();
		return 0;
	}
	pFileBufferBase = pFileBuffer;
	memset(pFileBuffer, 0, fileSize);

	// Copy EXE to Heap
	fseek(pTargetFile, 0, SEEK_SET);
	
	unsigned int whileCounter = 0;
	do {
		*pFileBuffer = fgetc(pTargetFile);
		pFileBuffer++;
		whileCounter++;
	} while (whileCounter != fileSize);
	pFileBuffer = pFileBufferBase;

	// Start inject
	gImageBase = *((unsigned int*)(pFileBuffer + gNtHeaderOffset + 0x18 + IMAGE_BASE_OFFSET -4) +1);
	pFileBuffer = pFileBufferBase;

	gAddressOfEntryPoint = *((unsigned int*)(pFileBuffer + gNtHeaderOffset + 0x18 + ADDRESS_OF_ENTRYPOINT_OFFSET - 4) + 1);
	pFileBuffer = pFileBufferBase;

	pFileBuffer += gSectionTableOffset + 1 * SECTIONTABLE_SIZE;	// Inject to second sectionTable
	gMisc = *((unsigned int*)(pFileBuffer + MISC_OFFSET - 4) +1);
	gVirtualAddress = *((unsigned int*)(pFileBuffer + VIRTUALADDRESS_OFFSET - 4) + 1);
	gPointerToRawData = *((unsigned int*)(pFileBuffer + POINTER_TO_RAWDATA_OFFSET - 4) + 1);
	pFileBuffer = pFileBufferBase;

	unsigned int convertedCallAddress = MESSAGEBOX_VADDRESS - (gImageBase + gVirtualAddress + gMisc + INSTRUCTION_PUSH_SIZE * 4 + INSTRUCTION_CALL_SIZE);
	unsigned int convertedEntryPoint = (gAddressOfEntryPoint + gImageBase) - (gImageBase + gVirtualAddress + gMisc + INSTRUCTION_PUSH_SIZE * 4 + INSTRUCTION_CALL_SIZE + INSTRUCTION_JMP_SIZE);

	unsigned char* pInjectCodeBuffer = (unsigned char*)malloc(INSTRUCTION_PUSH_SIZE * 4 + INSTRUCTION_CALL_SIZE + INSTRUCTION_JMP_SIZE);
	if (pInjectCodeBuffer == NULL) {
		printf("Fail to allocating heap memory");
		getchar();
		return 0;
	}
	unsigned char* pInjectCodeBufferBase = pInjectCodeBuffer;

	unsigned char InjectCodeArray[18] = {
		0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00,
		0x0E8, (unsigned char)convertedCallAddress, (unsigned char)(convertedCallAddress >> 8), (unsigned char)(convertedCallAddress >> 16), (unsigned char)(convertedCallAddress >> 24),
		0x0E9, (unsigned char)convertedEntryPoint, (unsigned char)(convertedEntryPoint >> 8), (unsigned char)(convertedEntryPoint >> 16), (unsigned char)(convertedEntryPoint >> 24)
	};

	// Write inject code to FileBuffer;
	pFileBuffer += gPointerToRawData + gMisc;
	memcpy(pFileBuffer, InjectCodeArray, 0x12);
	pFileBuffer = pFileBufferBase;

	// Change the addressOfEntryPoint
	pFileBuffer += gNtHeaderOffset + 0x18 + ADDRESS_OF_ENTRYPOINT_OFFSET;
	unsigned int newEntryPoint = gVirtualAddress + gMisc;
	whileCounter = 0;
	do {
		*pFileBuffer = (unsigned char)(newEntryPoint >> whileCounter * 8);
		pFileBuffer++;
		whileCounter++;
	} while (whileCounter != 4);
	pFileBuffer = pFileBufferBase;

	// FileBuffer to Disk
	FILE* pNewTargetFile = fopen("C:\\Users\\DEBUG\\Desktop\\InjectedProgram.exe", "wb");
	fwrite(pFileBuffer, BYTE, fileSize, pNewTargetFile);
	fclose(pNewTargetFile);

	printf("Inject Success!");
	getchar();
	return 0;
}

unsigned int fnGetFileSize(_IN_ FILE* pTargetFile) {
	unsigned char* pDosHeaderBuffer = NULL;
	unsigned char* pNtHeaderBuffer = NULL;
	unsigned char* pNtHeaderBufferBase = NULL;
	unsigned char* pSectionTable = NULL;
	unsigned char* pSectionTableBase = NULL;

	pDosHeaderBuffer = (unsigned char*)malloc(0x40);
	if (pDosHeaderBuffer == NULL) {
		printf("Fail to allocating heap memory");
		getchar();
		return 0;
	}
	memset(pDosHeaderBuffer, 0, 0x40);

	// Copy DosHeader to Heap
	unsigned int whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);
	do {
		*pDosHeaderBuffer = fgetc(pTargetFile);
		pDosHeaderBuffer++;
		whileCounter++;
	} while (whileCounter != 0x40);
	pDosHeaderBuffer -= 0x40;

	gNtHeaderOffset = (unsigned short)*(pDosHeaderBuffer + 0x3C);
	free(pDosHeaderBuffer);

	// Copy Nt-Header to Heap
	pNtHeaderBuffer = (unsigned char*)malloc(NT_HEADER_SIZE);
	if (pNtHeaderBuffer == NULL) {
		printf("Fail to allocating heap memory");
		getchar();
		return 0;
	}
	memset(pNtHeaderBuffer, 0, NT_HEADER_SIZE);
	pNtHeaderBufferBase = pNtHeaderBuffer;

	fseek(pTargetFile, gNtHeaderOffset, SEEK_SET);

	whileCounter = 0;
	do {
		*pNtHeaderBuffer = fgetc(pTargetFile);
		pNtHeaderBuffer++;
		whileCounter++;
	} while (whileCounter != NT_HEADER_SIZE);
	pNtHeaderBuffer = pNtHeaderBufferBase;

	gNumberOfSection = (unsigned short)*(pNtHeaderBuffer + NUMBER_OF_SECTION_OFFSET);
	free(pNtHeaderBuffer);

	gSectionTableOffset = gNtHeaderOffset + 0x18 + SIZEOF_OPTION_HEADER;

	pSectionTable = (unsigned char*)malloc(SECTIONTABLE_SIZE * gNumberOfSection);
	if (pSectionTable == NULL) {
		printf("Fail to allocating heap memory");
		getchar();
		return 0;
	}
	pSectionTableBase = pSectionTable;
	memset(pSectionTable, 0, SECTIONTABLE_SIZE * gNumberOfSection);

	fseek(pTargetFile, gSectionTableOffset, SEEK_SET);
	whileCounter = 0;
	do {
		*pSectionTable = fgetc(pTargetFile);
		pSectionTable++;
		whileCounter++;
	} while (whileCounter != SECTIONTABLE_SIZE * gNumberOfSection);
	pSectionTable = pSectionTableBase;

	// TotalFileSize = lastSectionTable.PointerToRawData + sizeOfRawData
	unsigned int sizeOfRawData = 0;
	unsigned int pointerToRawData = 0;

	pSectionTable += SIZEOF_RAWDATA_OFFSET + SECTIONTABLE_SIZE * (gNumberOfSection -1);

	sizeOfRawData = *((unsigned int*)(pSectionTable -4) +1);
	pointerToRawData = *((unsigned int*)((pSectionTable + DWORD) -4) +1);
	pSectionTable = pSectionTableBase;

	free(pSectionTable);

	return sizeOfRawData + pointerToRawData;
}