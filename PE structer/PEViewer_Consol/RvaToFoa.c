#include "RvaToFoa.h"

// pointerToRawData, FileAlignment, ImageAlignment, VirtualAddress, numberOfSection
unsigned int RvaToFoa(_IN_ FILE* pTargetFile, _IN_ unsigned int RvaAddress) {
	int whileCounter = 0;

	unsigned short NtHeaderOffset = 0;
	do {
		NtHeaderOffset <<= (8 * whileCounter);
		fseek(pTargetFile, 0x3C + (1 - whileCounter), SEEK_SET);
		NtHeaderOffset |= fgetc(pTargetFile);
		whileCounter++;
	} while (whileCounter != 2);
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	// Check is PE
	fseek(pTargetFile, NtHeaderOffset, SEEK_SET);
	unsigned short peSignature = 0;

	do {
		peSignature <<= (8 * whileCounter);
		fseek(pTargetFile, NtHeaderOffset + (1 - whileCounter), SEEK_SET);
		peSignature |= fgetc(pTargetFile);
		whileCounter++;
	} while (whileCounter != 2);
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	if (!(peSignature == 0x4550)) {
		printf("This is not a valid PE file!\n");
		return 0;
	}

	// check is 32bit exe
	unsigned short sizeOfOptionalHeader = 0;
	do {
		sizeOfOptionalHeader <<= (8 * whileCounter);
		fseek(pTargetFile, NtHeaderOffset + 0x14 + (1 - whileCounter), SEEK_SET);
		sizeOfOptionalHeader |= fgetc(pTargetFile);
		whileCounter++;
	} while (whileCounter != 2);
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	if (!(sizeOfOptionalHeader == 0x0E0)) {
		printf("64Bit NOT support!\n");
		return 0;
	}

	// Start convert
	// Validation
	unsigned int sizeOfImage = 0;
	do {
		if(whileCounter != 0)
			sizeOfImage <<= 8;

		fseek(pTargetFile, NtHeaderOffset + 0x50 + (3 - whileCounter), SEEK_SET);
		sizeOfImage |= fgetc(pTargetFile);
		whileCounter++;
	} while (whileCounter != 4);
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	if (RvaAddress > sizeOfImage) {
		printf("Invalid RVA Address (oversize)!\n");
		return 0;
	}

	unsigned int firstSectionTableRVA = 0;
	do {
		if (whileCounter != 0)
			firstSectionTableRVA <<= 8;

		fseek(pTargetFile, NtHeaderOffset + 0x18 + sizeOfOptionalHeader + (3 - whileCounter) + VIRTUAL_ADDRESS_OFFSET, SEEK_SET);
		firstSectionTableRVA |= fgetc(pTargetFile);
		whileCounter++;
	} while (whileCounter != 4);
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	unsigned int sizeOfHeader = 0;
	do {
		if (whileCounter != 0)
			sizeOfHeader <<= 8;

		fseek(pTargetFile, NtHeaderOffset + 0x54 + (3 - whileCounter), SEEK_SET);
		sizeOfHeader |= fgetc(pTargetFile);
		whileCounter++;
	} while (whileCounter != 4);
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	unsigned int FoaAddress = 0;
	if (RvaAddress < firstSectionTableRVA && RvaAddress > sizeOfHeader) {
		printf("Invalid RVA Address (unknow area)!\n");
		return 0;
	}
	else if (RvaAddress <= sizeOfHeader) {
		FoaAddress = RvaAddress;
		return FoaAddress;
	}

	// Get numberOfSection
	unsigned int numberOfSection = 0;
	do {
		numberOfSection <<= (8 * whileCounter);
		fseek(pTargetFile, NtHeaderOffset + 0x6 + (1 - whileCounter), SEEK_SET);
		numberOfSection |= fgetc(pTargetFile);
		whileCounter++;
	} while (whileCounter != 2);
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	// Get sectionTable Offset
	unsigned int sectionTableOffset = NtHeaderOffset + 0x18 + sizeOfOptionalHeader;

	// sectionTable Buffer
	unsigned int totalSectionTableSize = 0x28 * numberOfSection;
	unsigned char* pSectionTable = (unsigned char*)malloc(totalSectionTableSize);
	memset(pSectionTable, 0, totalSectionTableSize);
	fseek(pTargetFile, sectionTableOffset, SEEK_SET);
	do {
		*pSectionTable = fgetc(pTargetFile);
		pSectionTable++;
		whileCounter++;
	} while (whileCounter != totalSectionTableSize);
	pSectionTable -= totalSectionTableSize;
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	// Check which section table of the RVA
	unsigned int virtualAddress = 0;
	unsigned int RvaAddressInSection = 0;
	do {
		virtualAddress = *((unsigned int*)(pSectionTable + VIRTUAL_ADDRESS_OFFSET + (0x28 * whileCounter) - DWORD) + 1);
		if (RvaAddress < virtualAddress) {
			RvaAddressInSection = whileCounter;
			virtualAddress = *((unsigned int*)(pSectionTable + VIRTUAL_ADDRESS_OFFSET + (0x28 * (whileCounter - 1)) - DWORD) + 1);
			break;
		}
		whileCounter++;
		if (whileCounter == numberOfSection && RvaAddressInSection == 0) {	// FOA in last section
			RvaAddressInSection = numberOfSection;
		}
	} while (whileCounter != numberOfSection);
	whileCounter = 0;

	// Convert to FOA
	unsigned int pointerToRawData = 0;

	pointerToRawData = *((unsigned int*)(pSectionTable + POINTER_TO_RAWDATA_OFFSET + (0x28 * (RvaAddressInSection - 1)) - DWORD) + 1);
	FoaAddress = (RvaAddress - virtualAddress) + pointerToRawData;

	free(pSectionTable);
	return FoaAddress;
};