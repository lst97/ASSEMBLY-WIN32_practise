/*
;|**********************************************************************;
;* Project           : ASSEMBLY-WIN32_practise (PE_Structure)
;*
;* Program name      : PEinfo.c, FileToImage.c, Source.c, PEInject.c
;*
;* Author            : SIO TOU LAI (laisiotou1997@gmail.com)
;*
;* Date created      : 11/09/2019
;*
;* Version           : internal
;*
;* Copyright         : GNU GENERAL PUBLIC LICENSE Version 3
;*
;* Purpose           : Get PE info from file only support 32bit compiled written MASM
;*
;* Revision History  :
;* Date        Author      Ref    Revision (Date in DDMMYYYY format)
;* 11092019    lst97       1      First release
;* 13092019    lst97       2      Add PE inject function
;* 14092019    lst97       3      Add add sectionTable function
;* 19092019    lst97       4      Add print export && reloc table
;*
;* Known Issue       :
;* - PEinfo not yet finish, can only read until section table.
;* - Display problem when print sectionName
;* - Only works with MASM compiled program (PE injector)
;* TODO              :
;* + Full PEinfo tool.
;* + PE vaildate (to see if it's a valid PE file)
;* + Translation.
;* & Any suggestion?
;*
;|**********************************************************************;
*/

#include "stdio.h"
#include "stdlib.h"
#include "PEinfo.h"
#include "FileToImage.h"
#include "Menu.h"
#include "PEInject.h"
#include "AddSectionTable.h"
#include "RvaToFoa.h"
#include "PrintExportDirectory.h"
#include "PrintRelocationTable.h"

int main() {
	int functionNumber = 0;
	FILE* pTargetFile = fopen("C:\\Users\\DEBUG\\Desktop\\dllDemo.dll", "rb");
	if (pTargetFile == NULL) {
		printf("ERROR! Fail to open the file!");
		getchar();
		return 0;
	}

	GetPEinfo(pTargetFile, false);

	while (1) {
		do {
			functionNumber = ShowMenu();
			if (functionNumber == -1)
				printf("Invalid input!\n\n");
		} while (functionNumber == -1);

		switch (functionNumber) {
		case 1:
			while (getchar() != '\n');
			system("cls");
			RvaToFoa(pTargetFile, 0x255);
			GetPEinfo(pTargetFile, true);
			system("cls");
			break;
		case 2:
			while (getchar() != '\n');
			system("cls");

			fnExtendAndSaveFile(pTargetFile);
			system("cls");
		case 3:
			while (getchar() != '\n');
			system("cls");

			fnPEInject(pTargetFile);
			system("cls");
			continue;

		case 4:
			while (getchar() != '\n');
			system("cls");

			fnAddSectionTable(pTargetFile);
			system("cls");
			break;

		case 5:
			while (getchar() != '\n');
			system("cls");

			PrintExportDirectory(pTargetFile);
			system("cls");
			break;

		case 6:
			while (getchar() != '\n');
			system("cls");

			PrintRelocationTable(pTargetFile);
			system("cls");
			break;
		}
	}
	
}