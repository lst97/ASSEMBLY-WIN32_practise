#include "PrintExportDirectory.h"

int PrintExportDirectory(FILE* pTargetFile) {
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

	// export directory offset
	unsigned int exportDirectoryRVA = 0;
	do {
		if(whileCounter != 0)
			exportDirectoryRVA <<= 8;

		fseek(pTargetFile, NtHeaderOffset + 0x18 + 0x60 + (3 - whileCounter), SEEK_SET);
		exportDirectoryRVA |= fgetc(pTargetFile);
		whileCounter++;
	} while (whileCounter != 4);
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	if (exportDirectoryRVA == 0) {
		printf("(NULL)\n");
		return 0;
	}

	unsigned int exportDirectorySize = 0;
	do {
		if (whileCounter != 0)
			exportDirectorySize <<= 8;

		fseek(pTargetFile, 0x64 + (3 - whileCounter), SEEK_SET);
		exportDirectorySize |= fgetc(pTargetFile);
		whileCounter++;
	} while (whileCounter != 4);
	whileCounter = 0;
	fseek(pTargetFile, 0, SEEK_SET);

	unsigned char* pExportDirectory = (unsigned char*)malloc(0x28);
	unsigned char* pExportDirectoryBase = pExportDirectory;
	memset(pExportDirectory, 0, 0x28);
	unsigned int exportDitectoryFOA = RvaToFoa(pTargetFile, exportDirectoryRVA);

	fseek(pTargetFile, exportDitectoryFOA, SEEK_SET);
	do {
		*pExportDirectory = fgetc(pTargetFile);
		pExportDirectory++;
		whileCounter++;
	} while (whileCounter != 0x28);
	whileCounter = 0;
	pExportDirectory = pExportDirectoryBase;

	unsigned int base = *((unsigned int*)(pExportDirectory + 0x10 - 4) + 1);
	unsigned int numberOfFunctions = *((unsigned int*)(pExportDirectory + 0x14 - 4) + 1);
	unsigned int numberOfNames = *((unsigned int*)(pExportDirectory + 0x18 - 4) + 1);
	unsigned int addressOfFunctionRVA = *((unsigned int*)(pExportDirectory + 0x1C - 4) + 1);
	unsigned int addressOfNamesRVA = *((unsigned int*)(pExportDirectory + 0x20 - 4) + 1);	// maybe wrong calc
	unsigned int addressOfNameOrdinalsRVA = *((unsigned int*)(pExportDirectory + 0x24 - 4) + 1);

	unsigned int addressOfFunctionFOA = RvaToFoa(pTargetFile, addressOfFunctionRVA);
	unsigned int addressOfNamesFOA = RvaToFoa(pTargetFile, addressOfNamesRVA);
	unsigned int addressOfNameOrdinalsFOA = RvaToFoa(pTargetFile, addressOfNameOrdinalsRVA);

	// Create heap for addressOfFunction Table
	unsigned int* pAddressOfFunctions = (unsigned int*)malloc(numberOfFunctions);
	memset(pAddressOfFunctions, 0, sizeof(int) * numberOfFunctions);
	unsigned int totalSizeInByte = 4 * numberOfFunctions;

	fseek(pTargetFile, addressOfFunctionFOA, SEEK_SET);
	do {
		*((unsigned char*)pAddressOfFunctions) = fgetc(pTargetFile);
		((unsigned char*)pAddressOfFunctions += 1);
		whileCounter++;
	} while (whileCounter != totalSizeInByte);
	pAddressOfFunctions -= numberOfFunctions;
	whileCounter = 0;
	totalSizeInByte = 0;

	// Create heap for addressOfName Table
	unsigned int* pAddressOfName = (unsigned int*)malloc(numberOfNames);
	memset(pAddressOfName, 0, sizeof(int) * numberOfNames);
	totalSizeInByte = 4 * numberOfNames;

	if (numberOfNames != 0) {
		fseek(pTargetFile, addressOfNamesFOA, SEEK_SET);
		do {
			*((unsigned char*)pAddressOfName) = fgetc(pTargetFile);
			((unsigned char*)pAddressOfName += 1);
			whileCounter++;
		} while (whileCounter != totalSizeInByte);
		pAddressOfName -= numberOfNames;
		whileCounter = 0;
		totalSizeInByte = 0;
	}

	// Create heap for AddressOfNameOrdinals Table
	unsigned short* pAddressOfNameOrdinals = (unsigned short*)malloc(numberOfNames);
	memset(pAddressOfNameOrdinals, 0, sizeof(int) * numberOfNames);
	totalSizeInByte = 2 * numberOfNames;

	if (numberOfNames != 0) {
		fseek(pTargetFile, addressOfNameOrdinalsFOA, SEEK_SET);
		do {
			*((unsigned char*)pAddressOfNameOrdinals) = fgetc(pTargetFile);
			((unsigned char*)pAddressOfNameOrdinals += 1);
			whileCounter++;
		} while (whileCounter != totalSizeInByte);
		pAddressOfNameOrdinals -= numberOfNames;
		whileCounter = 0;
		totalSizeInByte = 0;
	}

	// Print export directory
	printf("-[0x%08X]struct _IMAGE_EXPORT_DIRECTORY\n|0x00,\t\tDWORD,\tCharacteristics,\t\t%08X,\t[]\n|0x04,\t\tDWORD,\tTimeDataStamp,\t\t\t%08X,\t[]\n|0x08,\t\tWORD,\tMajorVersion,\t\t\t%04X,\t\t[]\n|0x0A,\t\tWORD,\tMinorVersion,\t\t\t%04X,\t\t[]\n|0x0C,\t\tDWORD,\tName,\t\t\t\t%08X,\t[]\n|0x10,\t\tDWORD,\tBase,\t\t\t\t%08X,\t[]\n|0x14,\t\tDWORD,\tNumberOfFunctions,\t\t%08X,\t[]\n|0x18,\t\tDWORD,\tNumberOfNames,\t\t\t%08X,\t[]\n|0x1C,\t\tDWORD,\tAddressOfFunctions,\t\t%08X,\t[]\n|0x20,\t\tDWORD,\tAddressOfNames,\t\t\t%08X,\t[]\n|0x24,\t\tDWORD,\tAddressOfNameOrdinals,\t\t%08X,\t[];\n|\n",
		exportDitectoryFOA, 
		*((unsigned int*)(pExportDirectory - 4) + 1),
		*((unsigned int*)(pExportDirectory + 0x4 - 4) + 1),
		*((unsigned short*)(pExportDirectory + 0x8 - 2) + 1),
		*((unsigned short*)(pExportDirectory + 0xA - 2) + 1),
		*((unsigned int*)(pExportDirectory + 0x0C - 4) + 1),
		*((unsigned int*)(pExportDirectory + 0x10 - 4) + 1),
		*((unsigned int*)(pExportDirectory + 0x14 - 4) + 1),
		*((unsigned int*)(pExportDirectory + 0x18 - 4) + 1),
		*((unsigned int*)(pExportDirectory + 0x1C - 4) + 1),
		*((unsigned int*)(pExportDirectory + 0x20 - 4) + 1),
		*((unsigned int*)(pExportDirectory + 0x24 - 4) + 1)
		);
	pExportDirectory = pExportDirectoryBase;

	// Print AddressOfFunctions
	printf("-[0x%08X]address_of_Function[%d]\n", addressOfFunctionFOA,	numberOfFunctions);
	do {
		printf("|0x%02X,\t\tDWORD,\taddress_of_function[%d],\t\t%08X,\t[]", 
			4 * whileCounter, whileCounter, *(pAddressOfFunctions + whileCounter)
			);
		whileCounter++;
		if (whileCounter == numberOfFunctions)
			printf(";\n|\n");
		else
			printf("\n");
	} while (whileCounter != numberOfFunctions);
	whileCounter = 0;

	// Print AddressOfName
	if (numberOfNames != 0) {
		printf("-[0x%08X]address_of_Name[%d]\n", addressOfNamesFOA, numberOfNames);
		do {
			printf("|0x%02X,\t\tDWORD,\taddress_of_Name[%d],\t\t%08X,\t[]",
				4 * whileCounter, whileCounter, *(pAddressOfName + whileCounter)
			);
			whileCounter++;
			if (whileCounter == numberOfFunctions)
				printf(";\n|\n");
			else
				printf("\n");
		} while (whileCounter != numberOfFunctions);
		whileCounter = 0;
	}
	else {
		printf("-[0x%08X]address_of_Name[%d]\n", addressOfNamesFOA, numberOfNames);
		printf("|(N/A)\n|\n");
	};

	// Print AddressOfNameOrdinals
	if (numberOfNames != 0) {
		printf("-[0x%08X]name_ordinal[%d]\n", addressOfNameOrdinalsFOA, numberOfNames);
		do {
			printf("|0x%02X,\t\tWORD,\tname_ordinal[%d],\t\t%04X,\t[]",
				2 * whileCounter, whileCounter, *(pAddressOfNameOrdinals + whileCounter)
			);
			whileCounter++;
			if (whileCounter == numberOfFunctions)
				printf(";\n|\n");
			else
				printf("\n");
		} while (whileCounter != numberOfFunctions);
		whileCounter = 0;
	}
	else {
		printf("-[0x%08X]name_ordinal[%d]\n", addressOfNameOrdinalsFOA, numberOfNames);
		printf("|(N/A)\n|\n");
	};

	// print readable value
	unsigned int ordinalsNum = 0;
	unsigned int functionNameOffset = 0;
	unsigned int nameOfFunctionStringSize = 0;
	
	unsigned char charBuffer = 0;
	unsigned int lastNameOfFunctionStringSize = 0;
	fseek(pTargetFile, RvaToFoa(pTargetFile, *(pAddressOfName + (numberOfNames -1))), SEEK_SET);
	do {
		charBuffer = fgetc(pTargetFile);
		if (charBuffer != 0)
			lastNameOfFunctionStringSize++;
		else
			break;
	} while (1);
	nameOfFunctionStringSize = RvaToFoa(pTargetFile, *(pAddressOfName + (numberOfNames - 1))) - RvaToFoa(pTargetFile, *pAddressOfName) + lastNameOfFunctionStringSize + 1;

	unsigned char* pNameOfFunctionStringBuffer = (unsigned char*)malloc(nameOfFunctionStringSize);
	memset(pNameOfFunctionStringBuffer, 0, nameOfFunctionStringSize);

	// copy string into string buffer
	fseek(pTargetFile, RvaToFoa(pTargetFile, *pAddressOfName), SEEK_SET);
	do {
		*pNameOfFunctionStringBuffer = fgetc(pTargetFile);
		pNameOfFunctionStringBuffer++;
		whileCounter++;
	} while (whileCounter != nameOfFunctionStringSize);
	pNameOfFunctionStringBuffer -= nameOfFunctionStringSize;
	whileCounter = 0;

	// Print the list (not include all case, need improve, example: N/A name)
	printf("\n+--------------------------------------------------------------------------+\n|Ordinal\t|RVA\t\t|Function\t\t\t\t\t|\n+--------------------------------------------------------------------------+\n");
	do{
		for (unsigned int offsetCounter = 0; offsetCounter < numberOfNames; offsetCounter++) {
			fseek(pTargetFile, addressOfNameOrdinalsFOA + 2 * offsetCounter, SEEK_SET);
			ordinalsNum = fgetc(pTargetFile);
			if (whileCounter == ordinalsNum) {
				functionNameOffset = offsetCounter;
				printf("|0x%04X,\t0x%08X,\t%s\t\n",(ordinalsNum + 1) - base, *(pAddressOfFunctions + offsetCounter), fnGetStringBufferOffset(pNameOfFunctionStringBuffer, ordinalsNum));
			}
		}
		whileCounter++;
	} while (whileCounter != numberOfFunctions);
	whileCounter = 0;
	printf("|(End of Export Directory)\n");

	getchar();
	return 0;

}

unsigned char* fnGetStringBufferOffset(_IN_ unsigned char* pStringBuffer, _IN_ unsigned int offset) {

	int whileCounter = 0;
	while (offset != 0) {
		if (*pStringBuffer == 0)
			offset--;
		pStringBuffer++;
		whileCounter++;
	}
	return pStringBuffer;
}