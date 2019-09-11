#include "stdio.h"
#include "stdlib.h"
#include "PEinfo.h"
#include "FileToImage.h"
#include "Menu.h"

int main() {
	int functionNumber = 0;
	FILE* pTargetFile = fopen("C:\\Users\\DEBUG\\Desktop\\PEStructure_SampleProgram.exe", "rb");

	GetPEinfo(pTargetFile, false);

	do {
		functionNumber = ShowMenu();
		if (functionNumber == -1) 
			printf("Invalid input!\n\n");
	} while (functionNumber == -1);

	switch (functionNumber) {
	case 1:
		while (getchar() != '\n');
		system("cls");

		GetPEinfo(pTargetFile, true);
		break;
	case 2:
		while (getchar() != '\n');
		system("cls");

		fnExtendAndSaveFile(pTargetFile);
		break;
	}
}