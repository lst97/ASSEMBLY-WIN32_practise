/*
;|**********************************************************************;
;* Project           : ASSEMBLY-WIN32_practise
;*
;* Program name      : FunctionCallAsmDemo.c
;*
;* Author            : SIO TOU LAI (laisiotou1997@gmail.com)
;*
;* Date created      : 25/08/2019
;*
;* Version           : internal
;*
;* Copyright         : GNU GENERAL PUBLIC LICENSE Version 3
;*
;* Purpose           : Demo with a document showing how function call works in Assembly language (Dissambled in VS2019 NOT OD)
;*
;* Revision History  :
;* Date        Author      Ref    Revision (Date in DDMMYYYY format)
;* 25082019    lst97       1      First release
;*
;* Known Issue       :
;*
;* TODO              :
;*
;|**********************************************************************;
*/

#include <stdio.h>
#include <stdlib.h>

int fnSum2(int, int);
int fnSum3(int, int, int);
int fuSum5(int, int, int, int, int);

int main() {

	fuSum5(1, 2, 3, 4, 5);
	return 0;
}

int fnSum2(int numA, int numB) {
	return numA + numB;
}

int fnSum3(int numA, int numB, int numC) {
	int temp;

	temp = fnSum2(numA, numB);
	return temp + numC;
}

int fuSum5(int numA, int numB, int numC, int numD, int numE) {

	int tempSum2 = fnSum2(numA, numB);
	int tempSum3 = fnSum3(numC, numD, numE);

	return tempSum2 + tempSum3;
}
