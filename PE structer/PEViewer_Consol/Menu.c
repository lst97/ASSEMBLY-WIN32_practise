#include "Menu.h"

int ShowMenu() {
	printf("1. Show PE info\n"
		"2. File(RAW) -> Image(Virtual memory) -> File (In memory buffer) -> Disk (PE inject)\n\n"
		"3. Inject MessageBox into a File (PE inject)\n\n"
		"4. Add an additional section table\n\n"
		"5. View Export table\n\n"
		"6. View Relocation table\n\n"
		"\\PEinfo>");

	int userInput;
	scanf("%d", &userInput);

	if (userInput >= 1 && userInput <= 6)
		return userInput;
	else return -1;
}