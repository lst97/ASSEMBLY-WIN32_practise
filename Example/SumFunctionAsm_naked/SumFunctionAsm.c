/*
;|**********************************************************************;
;* Project           : ASSEMBLY-WIN32_practise
;*
;* Program name      : SumFunctionAsm.c
;*
;* Author            : SIO TOU LAI (laisiotou1997@gmail.com)
;*
;* Date created      : 25/08/2019
;*
;* Version           : internal
;*
;* Copyright         : GNU GENERAL PUBLIC LICENSE Version 3
;*
;* Purpose           : Writting a naked function to add 5 num.
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

int __declspec(naked) fnSum_Asm(int x, int y, int z) {

	__asm {

		// Init
		PUSH EBP
		MOV EBP, ESP
		SUB ESP, 0x0C0

		PUSH EBX
		PUSH ESI
		PUSH EDI

		LEA EDI, DWORD PTR DS : [EBP - 0xC0]
		MOV EAX, 0xCCCCCCCC
		MOV ECX, 0x30
		REP STOSD

		// function
		MOV DWORD PTR ES : [EBP - 0x4], 0x2
		MOV DWORD PTR ES : [EBP - 0x8], 0x3
		MOV DWORD PTR ES : [EBP - 0x0C], 0x4

		MOV EAX, DWORD PTR DS : [EBP + 0x8]
		ADD EAX, DWORD PTR DS : [EBP + 0x0C]
		ADD EAX, DWORD PTR DS : [EBP + 0x10]
		ADD EAX, DWORD PTR DS : [EBP - 0x4]
		ADD EAX, DWORD PTR DS : [EBP - 0x8]
		ADD EAX, DWORD PTR DS : [EBP - 0x0C]

		// return
		POP EDI
		POP ESI
		POP EBX
		ADD ESP, 0x0C0
		POP EBP
		RET
	}
}

int main() {

	fnSum_Asm(1, 2, 3);
	return 0;
}

//int fnSum(int x, int y, int z) {
//	int a = 2;
//	int b = 3;
//	int c = 4;
//
//	return x + y + z + a + b + c;
//}
