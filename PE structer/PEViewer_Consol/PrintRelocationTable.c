#include "PrintRelocationTable.h"

int PrintRelocationTable(FILE* pTargetFile) {
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

	// Relocation table RVA
	unsigned int relocationTalbeRVA = 0;
	do {
		if (whileCounter != 0)
			relocationTalbeRVA <<= 8;

		fseek(pTargetFile, NtHeaderOffset + 0x18 + 0x60 + 0x28 + (3 - whileCounter), SEEK_SET);
		relocationTalbeRVA |= fgetc(pTargetFile);
		whileCounter++;
	} while (whileCounter != 4);
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	// get firstSizeOfBlock value
	unsigned int relocationTableSize = 0;
	do {
		if (whileCounter != 0)
			relocationTableSize <<= 8;

		fseek(pTargetFile, NtHeaderOffset + 0x18 + 0x60 + 0x2C + (3 - whileCounter), SEEK_SET);
		relocationTableSize |= fgetc(pTargetFile);
		whileCounter++;
	} while (whileCounter != 4);
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	if (relocationTalbeRVA == 0 && relocationTableSize == 0) {
		printf("(NULL)\n");
		return 0;
	}

	unsigned int relocationTalbeFOA = RvaToFoa(pTargetFile, relocationTalbeRVA);
	unsigned char* pRelocationTableBuffer = 0;
	unsigned char* pRelocationTableBufferBase = 0;
	pRelocationTableBuffer = (unsigned char*)malloc(relocationTableSize);
	pRelocationTableBufferBase = pRelocationTableBuffer;
	memset(pRelocationTableBuffer, 0, relocationTableSize);

	fseek(pTargetFile, relocationTalbeFOA, SEEK_SET);
	do {
		*pRelocationTableBuffer = fgetc(pTargetFile);
		pRelocationTableBuffer++;
		whileCounter++;
	} while (whileCounter != relocationTableSize);
	whileCounter = 0;
	pRelocationTableBuffer = pRelocationTableBufferBase;

	// Print
	unsigned int SizeOfBlock_temp = 0;
	unsigned int VirtualAddress_temp = *pRelocationTableBufferBase;

	unsigned int BlockItemsValue_temp = 0;
	unsigned int BlockItemsRVA_temp = 0;
	unsigned int BlockItemsOffset_temp = 0;
	unsigned int BlockItemsType_temp = 0;
	unsigned int BlockCounter = 0;
	int SizeOfBlockCounter = 0;
	do {
		printf("-[0x%08X]struct _IMAGE_BASERELOC[%d]\n", relocationTalbeFOA + SizeOfBlock_temp * 2, BlockCounter);
		VirtualAddress_temp = *((unsigned int*)(pRelocationTableBuffer - 4) + 1);
		SizeOfBlock_temp = *((unsigned int*)(pRelocationTableBuffer + 4 - 4) + 1);
		printf("|0x00,\t\tDWORD,\tVirtualAddress,\t\t%08X,\t[]\n|0x04,\t\tDWORD,\tSizeOfBlock,\t\t%08X,\t[]\n|\n",
			VirtualAddress_temp,
			SizeOfBlock_temp
		);
		
		printf("++Block_Items\n +--------------------------------------------------------------------------+\n |Index\t\t|RVA\t\t|Value\t\t|Type\n +--------------------------------------------------------------------------+\n");
		SizeOfBlock_temp /= 2;
		pRelocationTableBuffer += 8;
		do {
			memcpy(&BlockItemsValue_temp, pRelocationTableBuffer, 2);
			BlockItemsOffset_temp = BlockItemsValue_temp;
			BlockItemsRVA_temp = VirtualAddress_temp + (BlockItemsOffset_temp & 0b0000111111111111);
			BlockItemsType_temp = BlockItemsValue_temp >> 12;
			printf(" |(%03d),\t0x%08X,\t0x%08X,\t%d\n",
				SizeOfBlockCounter,
				BlockItemsRVA_temp,
				BlockItemsOffset_temp,
				BlockItemsType_temp
				);
			SizeOfBlockCounter++;
			pRelocationTableBuffer += 2;
		} while (SizeOfBlockCounter != (SizeOfBlock_temp - 4));
		SizeOfBlockCounter = 0;
		printf(" |(End_Of_Block_Items)\n |\n");

		whileCounter += SizeOfBlock_temp * 2;
		BlockCounter++;
	} while (whileCounter != relocationTableSize);
	whileCounter = 0;
	BlockCounter = 0;
	pRelocationTableBuffer = pRelocationTableBufferBase;


	getchar();
	return 0;
}