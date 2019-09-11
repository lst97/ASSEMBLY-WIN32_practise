#include "PEinfo.h"

// PEviewe
unsigned int DosNtHeaderValue[57] = { 0 };
unsigned char* pSectionTable = 0;	// Used by FileToImage.c->fnGetFileSize(), FileToImage.c->fnExtendEXE()
unsigned int* pSectionTableBuffer = 0;	// Used by fnPrintPEinfo(), works as DosNtHeaderValue, an array storeing the value
unsigned int* pSectionNameBuffer = 0;	// Used by fnPrintPEinfo(), works as DosNtHeaderValue, an array storeing the value

unsigned char* pDosNtHeader = NULL;	// Used to be free from FileToImage.c

/*Make sure the code only run once.*/
bool isPEgeted = false;

int GetPEinfo(FILE* pTargetFile, bool PrintPEinfo) {
	if (isPEgeted == false) {
		unsigned int readByte = 0x400;	// Most of header is 0x400 by default.
		unsigned int readByteCounter = readByte;
		unsigned char* pDosNtHeaderBase = NULL;

		pDosNtHeader = (unsigned char*)malloc(sizeof(char) * readByte);
		if (pDosNtHeader == NULL) {
			printf("Fail to allocating heap memory");
			getchar();
			return 0;
		}
		pDosNtHeaderBase = pDosNtHeader;

		// copy exe content to heap
		while (readByteCounter != 0) {
			*pDosNtHeader = fgetc(pTargetFile);
			pDosNtHeader++;
			readByteCounter--;
		}

		// Size Table
		unsigned char dosHeaderSize[19] = {
			WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD, WORD * 4, WORD, WORD, WORD * 10, DWORD // (+0x40)
		};
		unsigned char ntHeaderSize[38] = {
			DWORD,	// Signature (+0x04)
			WORD, WORD, DWORD, DWORD, DWORD, WORD, WORD,	// _IMAGE_FILE_HEADER (+0x14)
			WORD, BYTE, BYTE, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, WORD, WORD, WORD, WORD, WORD, WORD, DWORD, DWORD, DWORD, DWORD, WORD, WORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD	// _IMAGE_OPTIONAL_HEADER (+0x64)
		};
		unsigned char sectionTableHeaderSize[10] = {
			BYTE * 8, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, WORD, WORD, DWORD
		};

		// DOS-Header
		unsigned int dataSize = 0;
		pDosNtHeader = pDosNtHeaderBase; // reset heap address to original.
		for (int sizeTableCounter = 0; sizeTableCounter < 19; sizeTableCounter++) {
			dataSize = (unsigned int)dosHeaderSize[sizeTableCounter];
			for (unsigned int readByteCounter = (unsigned int)dosHeaderSize[sizeTableCounter]; readByteCounter > 0; readByteCounter--) {
				DosNtHeaderValue[sizeTableCounter] |= *(pDosNtHeader + (dataSize - 1));
				if (readByteCounter != 1)
					DosNtHeaderValue[sizeTableCounter] <<= 8;
				dataSize--;
			}
			pDosNtHeader += dosHeaderSize[sizeTableCounter];
		}

		// Goto NT-Header
		unsigned char* pNtHeader = NULL;
		pDosNtHeader = pDosNtHeaderBase;
		pNtHeader = pDosNtHeader + DosNtHeaderValue[18];

		// NT-Header
		dataSize = 0;
		for (int ntHeaderCounter = 0; ntHeaderCounter < 38; ntHeaderCounter++) {
			dataSize = (unsigned int)ntHeaderSize[ntHeaderCounter];
			for (unsigned int readByteCounter = (unsigned int)ntHeaderSize[ntHeaderCounter]; readByteCounter > 0; readByteCounter--) {
				DosNtHeaderValue[ntHeaderCounter + 19] |= *(pNtHeader + (dataSize - 1));
				if (readByteCounter != 1)
					DosNtHeaderValue[ntHeaderCounter + 19] <<= 8;
				dataSize--;
			}
			pNtHeader += ntHeaderSize[ntHeaderCounter];
		}

		unsigned int sizeOfOptionalHeader = DosNtHeaderValue[25];
		// Goto NT-Header
		pNtHeader = pDosNtHeaderBase;
		pNtHeader += DosNtHeaderValue[18];

		// Section Header address = Signature + Standard PE + Optional PE header (in-byte)
		pNtHeader += 0x04 + 0x14 + sizeOfOptionalHeader;
		unsigned int numberOfSection = DosNtHeaderValue[21];

		// Allocate Total Section Table size
		pSectionTableBuffer = (unsigned int*)malloc(sizeof(char) * numberOfSection * 0x28);
		if (pSectionTableBuffer == NULL) {
			printf("Fail to allocating heap memory");
			getchar();
			return 0;
		}
		memset(pSectionTableBuffer, 0, sizeof(char) * numberOfSection * 0x28);

		// Allocate buffer to store each section table name
		pSectionNameBuffer = (unsigned int*)malloc(numberOfSection * sizeof(int) * 2);
		if (pSectionNameBuffer == NULL) {
			printf("Fail to allocating heap memory");
			getchar();
			return 0;
		}
		unsigned int* pSectionNameBufferBase = pSectionNameBuffer;

		// Storage section table name
		for (unsigned int sectionCounter = numberOfSection; sectionCounter > 0; sectionCounter--) {
			for (unsigned int valuePosistion = 0; valuePosistion < 10; valuePosistion++) {
				dataSize = (unsigned int)sectionTableHeaderSize[valuePosistion];
				if (valuePosistion == 0) {
					memcpy(pSectionNameBuffer, pNtHeader, sizeof(unsigned int) * 2);
					pSectionNameBuffer += 2;
					pNtHeader += sectionTableHeaderSize[valuePosistion];
					continue;
				}
				for (unsigned int readByteCounter = dataSize; readByteCounter > 0; readByteCounter--) {
					*(pSectionTableBuffer + valuePosistion + (10 * -(sectionCounter - numberOfSection))) |= *(pNtHeader + (dataSize - 1));
					if (readByteCounter != 1)
						* (pSectionTableBuffer + valuePosistion + (10 * -(sectionCounter - numberOfSection))) <<= 8;
					dataSize--;
				}
				pNtHeader += sectionTableHeaderSize[valuePosistion];
			}
		}
		pSectionNameBuffer = pSectionNameBufferBase;

		// Set pointer to First posistion of the section table
		pNtHeader = pDosNtHeaderBase;
		pNtHeader += DosNtHeaderValue[18];
		pSectionTable = pNtHeader + 0x04 + 0x14 + sizeOfOptionalHeader;

		isPEgeted = true;
	}
	if (PrintPEinfo == true)
		fnPrintPEinfo();

	return 0;
}

int fnPrintPEinfo() {
	unsigned int numberOfSection = DosNtHeaderValue[21];

	printf("-struct _IMAGE_DOS_HEADER (DOS Header)\n|+----------------------------------------------------------------------------------------+\n||ADDRESS\t|TYPE\t|Name\t\t|Value\t|Discription\t\t\t\t\t|\n|+----------------------------------------------------------------------------------------+\n|0x00,\t\tWORD,\te_magic,\t%04X,\t[MZ tag]*\n|0x02,\t\tWORD,\te_cblp,\t\t%04X,\t[Bytes on last page]\n|0x04,\t\tWORD,\te_cp,\t\t%04X,\t[Pages in file]\n|0x06,\t\tWORD,\te_crlc,\t\t%04X,\t[Relocations]\n|0x08,\t\tWORD,\te_cparhdr,\t%04X,\t[Size of header]\n|0x0A,\t\tWORD,\te_minalloc,\t%04X,\t[Minimum memory]\n|0x0C,\t\tWORD,\te_maxalloc,\t%04X,\t[Maximum memory]\n|0x1E,\t\tWORD,\te_ss,\t\t%04X,\t[Initial SS value]\n|0x10,\t\tWORD,\te_sp,\t\t%04X,\t[Initial SP value]\n|0x12,\t\tWORD,\te_csum,\t\t%04X,\t[Checksum]\n|0x14,\t\tWORD,\te_ip,\t\t%04X,\t[Initial IP value]\n|0x16,\t\tWORD,\te_cs,\t\t%04X,\t[Initial CS value]\n|0x18,\t\tWORD,\te_lfarlc,\t%04X,\t[Table offset]\n|0x1A,\t\tWORD,\te_ovno,\t\t%04X,\t[Overlay number]\n|0x1C,\t\tWORD,\te_res[4],\t%04X,\t[Reserved words]\n|0x24,\t\tWORD,\te_oemid,\t%04X,\t[OEM identifier]\n|0x26,\t\tWORD,\te_oeminfo,\t%04X,\t[OEM information]\n|0x28,\t\tWORD,\te_res2[10],\t%04X,\t[OEM Reserved words]\n|0x3C,\t\tWORD,\te_lfanew,\t%04X,\t[PE Header address]*;\n|(END OF DOS-HEADER)\n\n-struct _IMAGE_NT_HEADER (NT Header)\n|0x00,\t\tDWORD,\tSignature,\t%04X,\t[PE Signature];\n|\n++[0x04]struct _IMAGE_FILE_HEADER (Standard PE Header)\n |0x00,\t\tWORD,\tMachine,\t\t%04X,\t\t[Identifies type of target machine]*\n |0x02,\t\tWORD,\tNumberOfSections,\t%04X,\t\t[Size of section table]*\n |0x04,\t\tDWORD,\tTimeDateStamp,\t\t%08X,\t[Indicates when the file was created (since 00:00 January 1, 1970)]*\n |0x08,\t\tDWORD,\tPointerToSynbolTable,\t%08X,\t[File offset of COFF symbol table]\n |0x0C,\t\tDWORD,\tNumberOfSymbols,\t%08X,\t[Number of entries in symbol table]\n |0x10,\t\tWORD,\tSizeOfOptionalHeader,\t%04X,\t\t[Size of optional header]*\n |0x12,\t\tWORD,\tCharacteristics,\t%04X,\t\t[Attributes of the file]*;\n |\n++[0x18] struct _IMAGE_OPTIONAL_HEADER (Optional PE Header)\n |0x00,\t\tWORD,\tMagic,\t\t\t\t%04X,\t\t[State of image file]*\n |0x02,\t\tBYTE,\tMajorLinkerVersion,\t\t%02X,\t\t[Linker major version number]\n |0x03,\t\tBYTE,\tMinorLinkerVersion,\t\t%02X,\t\t[Linker minor cersion number]\n |0x04,\t\tDWORD,\tSizeOfCode,\t\t\t%08X,\t[Size of code (.text) section]*\n |0x08,\t\tDWORD,\tSizeOfInitializedData,\t\t%08X,\t[Size of Initialized data section]*\n |0x0C,\t\tDWORD,\tSizeOfUninitializedData,\t%08X,\t[Size of uninitialized data section (BSS)]*\n |0x10,\t\tDWORD,\tAddressOfEntryPoint,\t\t%08X,\t[Program entry point]*\n |0x14,\t\tDWORD,\tBaseOfCode,\t\t\t%08X,\t[Image base of .text section]*\n |0x18,\t\tDWORD,\tBaseOfData,\t\t\t%08X,\t[Image base of .data section]*\n |0x1C,\t\tDWORD,\tImageBase,\t\t\t%08X,\t[Preferred address of first byte of image]*\n |0x20,\t\tDWORD,\tSectionAlignment,\t\t%08X,\t[Alignment (in bytes) of sections]*\n |0x24,\t\tDWORD,\tFileAlignment,\t\t\t%08X,\t[Alignment factor that is used to align raw data]*\n |0x28,\t\tWORD,\tMajorOperatingSystemVersion,\t%04X,\t\t[Major required operating system]\n |0x2A,\t\tWORD,\tMinoroperatingSystemVersion,\t%04X,\t\t[Minor required operating system]\n |0x2C,\t\tWORD,\tMajorImageVersion,\t\t%04X,\t\t[Major version number of image]\n |0x2E,\t\tWORD,\tMinorImageVersion,\t\t%04X,\t\t[Minor version number of image]\n |0x30,\t\tWORD,\tMajorSubsystemVersion,\t\t%04X,\t\t[Major version number of subsystem]\n |0x32,\t\tWORD,\tMinorSubsystemVersion,\t\t%04X,\t\t[Minor version number of subsystem]\n |0x34,\t\tDWORD,\tWin32VersionValue,\t\t%08X,\t[Reserved, must be zero]\n |0x38,\t\tDWORD,\tSizeOfImage,\t\t\t%08X,\t[Size of image, including all headers]*\n |0x3C,\t\tDWORD,\tSizeOfHeaders,\t\t\t%08X,\t[Size of MS-DOS stub, PE header, section headers]*\n |0x40,\t\tDWORD,\tCheckSum,\t\t\t%08X,\t[For validation at load time]*\n |0x44,\t\tWORD,\tSubsystem,\t\t\t%04X,\t\t[Subsystem that required to run this image]\n |0x46,\t\tWORD,\tDllCharacteristics,\t\t%04X,\t\t[See DLL Characteristics]\n |0x48,\t\tDWORD,\tSizeOfStackReserve,\t\t%08X,\t[Size of stack to reserve]*\n |0x4C,\t\tDWORD,\tSizeOfStackCommit,\t\t%08X,\t[Size of stack to commit]*\n |0x50,\t\tDWORD,\tSizeOfHeapReserve,\t\t%08X,\t[Size of local heap to reserve]*\n |0x54,\t\tDWORD,\tSizeOfHeapCommit,\t\t%08X,\t[Size of local heap to commit]*\n |0x58,\t\tDWORD,\tLoaderFlags,\t\t\t%08X,\t[Reserved, must be zero]\n |0x5C,\t\tDWORD,\tNumberOfRvaAndSizes,\t\t%08X,\t[Number of data-directory entries in remainder of optional header];\n |\n+++[0x60]_IMAGE_DATA_DIRECTORY DataDirectory[16]\n   |(END OF NT-HEADER) // TO-DO\n   |\n\n",
		DosNtHeaderValue[0], DosNtHeaderValue[1], DosNtHeaderValue[2], DosNtHeaderValue[3], DosNtHeaderValue[4], DosNtHeaderValue[5], DosNtHeaderValue[6], DosNtHeaderValue[7], DosNtHeaderValue[8], DosNtHeaderValue[9], DosNtHeaderValue[10], DosNtHeaderValue[11], DosNtHeaderValue[12], DosNtHeaderValue[13], DosNtHeaderValue[14], DosNtHeaderValue[15], DosNtHeaderValue[16], DosNtHeaderValue[17], DosNtHeaderValue[18],
		DosNtHeaderValue[19],
		DosNtHeaderValue[20], DosNtHeaderValue[21], DosNtHeaderValue[22], DosNtHeaderValue[23], DosNtHeaderValue[24], DosNtHeaderValue[25], DosNtHeaderValue[26],
		DosNtHeaderValue[27], DosNtHeaderValue[28], DosNtHeaderValue[29], DosNtHeaderValue[30], DosNtHeaderValue[31], DosNtHeaderValue[32], DosNtHeaderValue[33], DosNtHeaderValue[34], DosNtHeaderValue[35], DosNtHeaderValue[36], DosNtHeaderValue[37], DosNtHeaderValue[38], DosNtHeaderValue[39], DosNtHeaderValue[40], DosNtHeaderValue[41], DosNtHeaderValue[42], DosNtHeaderValue[43], DosNtHeaderValue[44], DosNtHeaderValue[45], DosNtHeaderValue[46], DosNtHeaderValue[47], DosNtHeaderValue[48], DosNtHeaderValue[49], DosNtHeaderValue[50], DosNtHeaderValue[51], DosNtHeaderValue[52], DosNtHeaderValue[53], DosNtHeaderValue[54], DosNtHeaderValue[55], DosNtHeaderValue[56]
	);

	for (unsigned int printCounter = 0; printCounter < numberOfSection; printCounter++) {
		// SectionName have display problem as it base on an interger pointer.
		printf("-typedef struct _IMAGE_SECTION_HEADER (Section Table [%-8s])\n|0x00,\t\tBYTE,\tName[IMAGE_SIZEOF_SHORT_NAME],\t%08X%08X,\t[An 8-byte, null-padded UTF-8 encoded string]\n|0x08,\t\tDWORD,\tPhysicalAddress/VirtualSize,\t%08X,\t\t[Total size of section when loaded into memory]\n|0x0C,\t\tDWORD,\tVirtualAddress,\t\t\t%08X,\t\t[First byte of section relative to image base]\n|0x10,\t\tDOWRD,\tSizeOfRawData,\t\t\t%08X,\t\t[Size of section on disk]\n|0x14,\t\tDWORD,\tPointerToRawData,\t\t%08X,\t\t[Pointer to first page of section within COFF file]\n|0x18,\t\tDWORD,\tPointerToRelocations,\t\t%08X,\t\t[Pointer to beginning of relocation entries for section]\n|0x1C,\t\tDOWRD,\tPointerToLinenumbers,\t\t%08X,\t\t[Pointer to beginning of line-number entries for section]\n|0x20,\t\tWORD,\tNumberOfRelocations,\t\t%04X,\t\t\t[Number of relocation entries for section]\n|0x22,\t\tWORD,\tNumberOfLinenumbers,\t\t%04X,\t\t\t[Number of line-number entries for section]\n|0x24,\t\tDWORD,\tCharacteristics,\t\t%08X,\t\t[Flags that describe the characteristics]\n|\n",
			(char*)(((char*)pSectionNameBuffer + (BYTE * 8) * printCounter)), *(pSectionNameBuffer + (WORD * (printCounter + 1) - 1)), *(pSectionNameBuffer + (WORD * printCounter)), *(pSectionTableBuffer + 1 + (SECTION_TABLE_SIZE * -(printCounter - numberOfSection))), *(pSectionTableBuffer + 2 + (SECTION_TABLE_SIZE * -(printCounter - numberOfSection))), *(pSectionTableBuffer + 3 + (SECTION_TABLE_SIZE * -(printCounter - numberOfSection))), *(pSectionTableBuffer + 4 + (SECTION_TABLE_SIZE * -(printCounter - numberOfSection))), *(pSectionTableBuffer + 5 + (SECTION_TABLE_SIZE * -(printCounter - numberOfSection))), *(pSectionTableBuffer + 6 + (SECTION_TABLE_SIZE * -(printCounter - numberOfSection))), *(pSectionTableBuffer + 7 + (SECTION_TABLE_SIZE * -(printCounter - numberOfSection))), *(pSectionTableBuffer + 8 + (SECTION_TABLE_SIZE * -(printCounter - numberOfSection))), *(pSectionTableBuffer + 9 + (SECTION_TABLE_SIZE * -(printCounter - numberOfSection)))
		);
	}

	free(pSectionNameBuffer);
	getchar();
	return 0;
}