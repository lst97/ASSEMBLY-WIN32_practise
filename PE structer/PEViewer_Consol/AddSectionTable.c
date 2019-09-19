#include "AddSectionTable.h"

int fnAddSectionTable(_IN_ FILE* pTargetFile) {
	struct SectionTable
	{
		unsigned char sectionName[7];
		unsigned int misc;
		unsigned int virtualAddress;
		unsigned int sizeOfRawData;
		unsigned int pointerToRawData;
		unsigned int pointerToRelocations;
		unsigned int pointerToLinenumbers;
		unsigned short numberOfRelocations;
		unsigned short numberOfLinenumbers;
		unsigned int characteristics;

	};

	unsigned int fileSize = 0;
	unsigned char* pFileBuffer = NULL;
	unsigned char* pFileBufferBase = NULL;
	unsigned char* pNewSectionTable = NULL;
	unsigned char* pNewSectionTableBase = NULL;

	unsigned int numOfSectionTableAllow = 0;

	fileSize = fnGetFileSize(pTargetFile);

	pFileBuffer = (unsigned char*)malloc(fileSize);
	if (pFileBuffer == NULL) {
		printf("Fail to allocating heap memory");
		getchar();
		return 0;
	}
	pFileBufferBase = pFileBuffer;
	memset(pFileBuffer, 0, fileSize);

	fseek(pTargetFile, 0, SEEK_SET);

	unsigned int whileCounter = 0;
	do {
		*pFileBuffer = fgetc(pTargetFile);
		pFileBuffer++;
		whileCounter++;
	} while (whileCounter != fileSize);
	pFileBuffer = pFileBufferBase;
	fclose(pTargetFile);

	// Add a new sectionTable
	pNewSectionTable = pFileBuffer + gNtHeaderOffset + 0x18 + SIZEOF_OPTION_HEADER + gNumberOfSection * SECTIONTABLE_SIZE;

	// Check oversize
	if (fnIsInjectTableOversize(pNewSectionTable)) {
		numOfSectionTableAllow = fnGetNewSectionTableAddress(pNewSectionTable, pFileBuffer, &pNewSectionTable);
		pNewSectionTableBase = pNewSectionTable;
		gNtHeaderOffset = 0x40;
	}

	// Get lastSection Table value
	unsigned int lastMisc = 0;
	unsigned int lastVirtualAddress = 0;
	unsigned int lastSizeOfRawData = 0;
	unsigned int lastPointerToRawData= 0;

	unsigned int sectionAlignment = *((unsigned int*)(pFileBuffer + (gNtHeaderOffset + 0x18 + SECTIONALIGNMENT_OFFSET - DWORD)) + 1);
	unsigned int fileAlignment = *((unsigned int*)(pFileBuffer + (gNtHeaderOffset + 0x18 + FILEALIGNMENT_OFFSET - DWORD)) + 1);

	fnGetLastSectionTableValue(pNewSectionTable - 0x28, &lastMisc, &lastVirtualAddress, &lastSizeOfRawData, &lastPointerToRawData);
	// process new sectiontable value
	struct SectionTable InjectSectionTable;
	strcpy(InjectSectionTable.sectionName, ".inject");
	InjectSectionTable.misc = 0x1000;
	InjectSectionTable.virtualAddress = lastVirtualAddress + (sectionAlignment * (lastMisc / (sectionAlignment + 1) + 1));
	InjectSectionTable.sizeOfRawData = 0x1000;
	InjectSectionTable.pointerToRawData = lastPointerToRawData + (fileAlignment * (lastSizeOfRawData / (fileAlignment + 1) + 1));
	InjectSectionTable.pointerToRelocations = 0;
	InjectSectionTable.pointerToLinenumbers = 0;
	InjectSectionTable.numberOfRelocations = 0;
	InjectSectionTable.numberOfLinenumbers = 0;
	InjectSectionTable.characteristics = CHARACTERISTICS_TEXT;

	// Put New section table into FileBuffer
	memcpy(pNewSectionTable, &InjectSectionTable, sizeof(InjectSectionTable));

	// Correction of numberOfSection
	memset(pFileBuffer + (gNtHeaderOffset + NUMBER_OF_SECTION_OFFSET), ++gNumberOfSection, 1);

	// Correction of sizeOfImage
	unsigned int oldSizeOfImage = *((unsigned int*)(pFileBuffer + (gNtHeaderOffset + 0x18 + SIZEOF_IMAGE_OFFSET - DWORD)) + 1);
	unsigned int newSizeOfImage = oldSizeOfImage += 0x1000;
	*((unsigned int*)(pFileBuffer + (gNtHeaderOffset + 0x18 + SIZEOF_IMAGE_OFFSET - DWORD)) + 1) = newSizeOfImage;

	// New FileBuffer heap
	unsigned char* pNewFileBuffer = (unsigned char*)malloc(fileSize + 0x1000);
	memset(pNewFileBuffer, 0, fileSize + 0x1000);
	memcpy(pNewFileBuffer, pFileBuffer, fileSize);
	free(pFileBuffer);

	// Back to Disk
	pTargetFile = fopen("C:\\Users\\DEBUG\\Desktop\\AddSectionTable.exe", "wb");
	fwrite(pNewFileBuffer, 1, fileSize + 0x1000, pTargetFile);
	free(pNewFileBuffer);
	fclose(pTargetFile);

	printf("Add section table SUCCESS!");
	getchar();
	return 0;
}

bool fnIsInjectTableOversize(_IN_ unsigned char* pNewSectionTable) {
	int wileCounter = 0;
	do {
		if (*pNewSectionTable != 0) {
			printf("Not enough space for adding a section table\nPlan B activaded.");
			return true;
		}
		pNewSectionTable++;
		wileCounter++;
	} while (wileCounter != 0x28 * 2);
	pNewSectionTable -= 0x28 * 2;

	return false;
}

unsigned int fnGetNewSectionTableAddress(_IN_ unsigned char* pSectionTable, _IN_ unsigned char* pFileBuffer, _OUT_ unsigned char** pNewSectionTable) {
	unsigned int junkDataSize = gNtHeaderOffset - SIZEOF_DOSHEADER;
	unsigned int numOfSectionTableAllow = (junkDataSize / 0x28) -1;

	// move NT-Header && all sectionTable to junkData
	memset(pFileBuffer + SIZEOF_DOSHEADER, 0, junkDataSize);
	memcpy(pFileBuffer + SIZEOF_DOSHEADER, pFileBuffer + gNtHeaderOffset, 0x18 + SIZEOF_OPTION_HEADER + gNumberOfSection * SECTIONTABLE_SIZE);
	memset(pFileBuffer + SIZEOF_DOSHEADER + 0x18 + SIZEOF_OPTION_HEADER + gNumberOfSection * SECTIONTABLE_SIZE, 0, junkDataSize);

	*pNewSectionTable = pFileBuffer + SIZEOF_DOSHEADER + 0x18 + SIZEOF_OPTION_HEADER + gNumberOfSection * SECTIONTABLE_SIZE;
	return numOfSectionTableAllow;
}

void fnGetLastSectionTableValue(_IN_ unsigned char* pSectionTable, _OUT_ unsigned int* pMisc, unsigned int* pVirtualAddress, unsigned int* pSizeOfRawData, unsigned int* pPointerToRawData) {
	*pMisc = *((unsigned int*)(pSectionTable + BYTE * 8 - DWORD) + 1);
	*pVirtualAddress = *((unsigned int*)(pSectionTable + VIRTUALADDRESS_OFFSET - DWORD) + 1);
	*pSizeOfRawData = *((unsigned int*)(pSectionTable + SIZEOF_RAWDATA_OFFSET - DWORD) + 1);
	*pPointerToRawData = *((unsigned int*)(pSectionTable + POINTER_TO_RAWDATA_OFFSET - DWORD) + 1);

	return;
}