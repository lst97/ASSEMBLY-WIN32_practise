#include "FileToImage.h"

int fnExtendAndSaveFile(FILE* pTargetFile) {
	unsigned char* pFileBuffer = NULL;
	unsigned char* pImageBuffer = NULL;
	unsigned char* pNewFileBuffer = NULL;
	
	// Create a full copy of the exe file into heap.
	pFileBuffer = fnCopyEXE(pTargetFile);
	fclose(pTargetFile);

	// Extend File to Image.
	pImageBuffer = fnExtendEXE(pFileBuffer);
	free(gpDosNtHeader);

	// Back to File format
	pNewFileBuffer = fnBackToFile(pImageBuffer);

	// Export to file
	FILE* pNewTargetFile = fopen("C:\\Users\\DEBUG\\Desktop\\NewSampleProgram.exe", "wb");
	unsigned int fileSize = fnGetFileSizeFromImage(pImageBuffer);
	free(pImageBuffer);

	fwrite(pNewFileBuffer, BYTE, BYTE * fileSize, pNewTargetFile);
	fclose(pNewTargetFile);

	getchar();
	return 0;
}

unsigned char* fnCopyEXE(FILE* pTargetFile) {
	unsigned int bufferSize = 0;
	unsigned int readByteCounter = 0;
	unsigned char* pFileBuffer = NULL;
	unsigned char* pFileBuffer_Base = NULL;

	bufferSize = fnGetFileSize();
	readByteCounter = bufferSize;
	pFileBuffer = (unsigned char*)malloc(sizeof(char) * bufferSize);
	if (pFileBuffer == NULL) {
		printf("Fail to allocating heap memory");
		getchar();
		return 0;
	}
	pFileBuffer_Base = pFileBuffer;

	// copy exe content to heap
	fseek(pTargetFile, 0, SEEK_SET);
	while (readByteCounter != 0) {
		*pFileBuffer = fgetc(pTargetFile);
		pFileBuffer++;
		readByteCounter--;
	}

	return pFileBuffer_Base;
}

unsigned int fnGetFileSize() {
	unsigned int totalRawSize = 0;

	unsigned int sizeOfHeader = gDosNtHeaderValue[47];
	unsigned int numberOfSection = gDosNtHeaderValue[21];
	unsigned int sectionTableOffset = BYTE * 8 + DWORD + DWORD;

	unsigned char* pFirstSectionTable = gpSectionTable;

	// File size = (SizeOfHeaders + all of the SizeOfRawData)
	for (unsigned int counter = numberOfSection; counter > 0; counter--) {
		totalRawSize += *((unsigned int*)pFirstSectionTable + (sectionTableOffset / DWORD));
		sectionTableOffset += 0x28;
	}
	totalRawSize += sizeOfHeader;

	return totalRawSize;
}

unsigned char* fnExtendEXE(char* pFileBuffer) {
	unsigned int sizeOfHeader = gDosNtHeaderValue[47];
	unsigned int sizeOfImage = gDosNtHeaderValue[46];
	unsigned int numberOfSection = gDosNtHeaderValue[21];

	unsigned int sizeOfRawData = 0;
	unsigned int pointerToRawData = 0;
	unsigned int Misc = 0;
	unsigned int virtualAddress = 0;

	unsigned char* pFirstSectionTable = gpSectionTable;
	unsigned char* pImageBuffer = NULL;

	pImageBuffer = (unsigned char*)malloc(sizeof(char) * sizeOfImage);
	if (pImageBuffer == NULL) {
		printf("Fail to allocating heap memory");
		getchar();
		return 0;
	}
	memset(pImageBuffer, 0, sizeof(char) * sizeOfImage);

	// Copy Header into ImageBuffer
	memcpy(pImageBuffer, pFileBuffer, sizeof(char) * sizeOfHeader);

	for (unsigned sectionCounter = 0; sectionCounter < numberOfSection; sectionCounter++) {
		sizeOfRawData = *(((unsigned int*)pFirstSectionTable + (BYTE * 8 + DWORD + DWORD) / (sizeof(int))) + (0x28 / sizeof(int)) * sectionCounter);
		pointerToRawData = *(((unsigned int*)pFirstSectionTable + (BYTE * 8 + DWORD + DWORD + DWORD) / (sizeof(int))) + (0x28 / sizeof(int)) * sectionCounter);
		Misc = *(((unsigned int*)pFirstSectionTable + ((BYTE * 8) / (sizeof(int)))) + (0x28 / sizeof(int)) * sectionCounter);
		virtualAddress = *(((unsigned int*)pFirstSectionTable + (BYTE * 8 + DWORD) / (sizeof(int))) + (0x28 / sizeof(int)) * sectionCounter);

		if (Misc < sizeOfRawData)
			memcpy(pImageBuffer + virtualAddress, pFileBuffer + pointerToRawData, sizeof(char) * Misc);
		else
			memcpy(pImageBuffer + virtualAddress, pFileBuffer + pointerToRawData, sizeof(char) * sizeOfRawData);
	}

	return pImageBuffer;
}

unsigned char* fnBackToFile(unsigned char* pImageBuffer) {
	unsigned int sizeOfHeader = 0;
	unsigned int numberOfSection = 0;
	unsigned int sizeOfRawData = 0;
	unsigned int pointerToRawData = 0;
	unsigned int virtualAddress = 0;
	unsigned int totalFileSize = 0;

	unsigned int DosNtHeaderOffset = *((unsigned int*)(pImageBuffer + 0x3C - DWORD) + 1);
	unsigned int sizeOfOptionalHeader = (unsigned int) * ((pImageBuffer + DosNtHeaderOffset) + 0x14);
	unsigned int sectionTableOffset = DosNtHeaderOffset + 0x04 + 0x14 + sizeOfOptionalHeader;;
	unsigned int virtualAddressOffset = BYTE * 8 + DWORD;
	unsigned int sizeOfRawDataOffset = BYTE * 8 + DWORD + DWORD;
	unsigned int pointerToRawDataOffset = BYTE * 8 + DWORD + DWORD + DWORD;

	unsigned char* pNewFileBuffer_Base = NULL;

	sizeOfHeader = *((unsigned int*)(pImageBuffer + DosNtHeaderOffset + 0x04 + 0x14 + 0x38) +1);

	totalFileSize = fnGetFileSizeFromImage(pImageBuffer);

	unsigned char* pNewFileBuffer = (unsigned char*)malloc(sizeof(char) * totalFileSize);
	if (pNewFileBuffer == NULL) {
		printf("Fail to allocating heap memory");
		getchar();
		return 0;
	}
	memset(pNewFileBuffer, 0, totalFileSize);
	pNewFileBuffer_Base = pNewFileBuffer;

	memcpy(pNewFileBuffer, pImageBuffer, sizeof(char) * sizeOfHeader);
	pNewFileBuffer += sizeOfHeader;

	numberOfSection = *((unsigned short*)(pNewFileBuffer_Base + DosNtHeaderOffset + 0x6 - WORD) +1);

	for (unsigned int sectionCounter = 0; sectionCounter < numberOfSection; sectionCounter++) {

		virtualAddress = *((unsigned int*)((pNewFileBuffer_Base + sectionTableOffset + virtualAddressOffset) + (0x28 * sectionCounter) - DWORD) + 1);
		sizeOfRawData = *((unsigned int*)((pNewFileBuffer_Base + sectionTableOffset + sizeOfRawDataOffset) + (0x28 * sectionCounter) - DWORD) +1);
		pointerToRawData = *((unsigned int*)((pNewFileBuffer_Base + sectionTableOffset + pointerToRawDataOffset) + (0x28 * sectionCounter) - DWORD) + 1);

		memcpy(pNewFileBuffer, pImageBuffer + virtualAddress, sizeof(char) * sizeOfRawData);
		pNewFileBuffer += sizeOfRawData;
	}
	return pNewFileBuffer_Base;
}

unsigned int fnGetFileSizeFromImage(unsigned char* pImageBuffer_Base) {
	unsigned char* pImageBuffer = pImageBuffer_Base;
	unsigned int DosNtHeaderOffset = *(pImageBuffer + 0x3C);
	unsigned int sizeOfHeader = *((int*)(pImageBuffer + DosNtHeaderOffset) + (0x56 / sizeof(int)));
	unsigned int numberOfSection = *((pImageBuffer + DosNtHeaderOffset) + 0x6); // *possible lost of data
	unsigned int sizeOfOptionalHeader = (int)*((pImageBuffer + DosNtHeaderOffset) + 0x14);
	unsigned int sizeOfRawData = BYTE * 8 + DWORD + DWORD;

	unsigned int sectionTableOffset = 0;
	unsigned int totalFileSize = 0;
	unsigned int totalSizeOfRawData = 0;

	// Section Header address = Signature + Standard PE + Optional PE header (in-byte)
	sectionTableOffset += DosNtHeaderOffset + 0x04 + 0x14 + sizeOfOptionalHeader;

	for (unsigned int sectionCounter = 0; sectionCounter < numberOfSection; sectionCounter++) {
		totalSizeOfRawData += *((unsigned int*)(pImageBuffer + sectionTableOffset + sizeOfRawData) + ((0x28 / sizeof(unsigned int)) * sectionCounter));
	}
	totalFileSize += sizeOfHeader + totalSizeOfRawData;

	return totalFileSize;
}