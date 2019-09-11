#include "Menu.h"

int ShowMenu() {
	printf("1. Show PE info\n"
		"2. File(RAW) -> Image(Virtual memory) -> File (In memory buffer) -> Disk (File formate)\n\n"
		"\\PEinfo>");

	int userInput;
	scanf("%d", &userInput);

	if (userInput >= 1 && userInput <= 2)
		return userInput;
	else return -1;
}