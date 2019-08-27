/*
;|**********************************************************************;
;* Project           : ASSEMBLY-WIN32_practise
;*
;* Program name      : LoopAndSwitch.c
;*
;* Author            : SIO TOU LAI (laisiotou1997@gmail.com)
;*
;* Date created      : 27/08/2019
;*
;* Version           : internal
;*
;* Copyright         : GNU GENERAL PUBLIC LICENSE Version 3
;*
;* Purpose           : Demonstrate how loop and switch works in assembly
;*
;* Revision History  :
;* Date        Author      Ref    Revision (Date in DDMMYYYY format)
;* 27082019    lst97       1      First release
;*
;* Known Issue       :
;*
;* TODO              :
;*
;|**********************************************************************;
*/

#include <stdio.h>
#include <stdlib.h>

void fnDoWhile();
void fnWhile();
void fnFor();
void fnSwitch();

int main() {

	fnDoWhile();
	fnWhile();
	fnFor();
	fnSwitch();

	getchar();

	return 0;
}

void fnDoWhile() {
	int counter = 0;
	do {
		printf("%d", counter);
		counter++;
	} while (counter < 10);
}

void fnWhile() {
	int counter = 0;
	while (counter < 10) {
		printf("%d", counter);
		counter++;
	}
}

void fnFor() {
	int counter = 0;

	for (counter; counter < 10; counter++) {
		printf("%d", counter);
	}
}

void fnSwitch() {
	int x = 55;
	// 4 possible compaination of switch.
	// (1)
	switch (x) {
	case 55:
		printf("55");
		break;
	case 2:
		printf("2");
		break;
	case 1:
		printf("1");
		break;
	case 0:
		printf("0");
		break;
	default:
		printf("default");
		break;
	}

	// (2)
	switch (x) {
	case 3:
		printf("3");
		break;
	case 2:
		printf("2");
		break;
	case 1:
		printf("1");
		break;
	case 0:
		printf("0");
		break;
	default:
		printf("default");
		break;
	}

	// (3)
	switch (x) {
	case 2:
		printf("2");
		break;
	case 1:
		printf("1");
		break;
	case 0:
		printf("0");
		break;
	default:
		printf("default");
		break;
	}

	// (4)
	switch (x) {
	case 0x1FF:
		printf("0x1FF");
		break;
	case 785:
		printf("785");
		break;
	case 1:
		printf("1");
		break;
	case -99:
		printf("0");
		break;
	default:
		printf("default");
		break;
	}
}
