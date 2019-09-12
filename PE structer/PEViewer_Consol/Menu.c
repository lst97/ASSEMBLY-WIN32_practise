#include "Menu.h"

int ShowMenu() {
	printf("1. Show PE info\n"
		"2. File(RAW) -> Image(Virtual memory) -> File (In memory buffer) -> Disk (PE inject)\n\n"
		"3. Inject MessageBox into a File (PE inject)\n\n"
		"\\PEinfo>");

	int userInput;
	scanf("%d", &userInput);

	if (userInput >= 1 && userInput <= 3)
		return userInput;
	else return -1;
}