;|**********************************************************************;
;* Project           : Basic_WIN32_ASSEMBLY
;*
;* Program name      : helloWorld.asm
;*
;* Author            : SIO TOU LAI (laisiotou1997@gmail.com)
;*
;* Date created      : 09/01/2019
;*
;* Version           : Internal
;*
;* Copyright         : GNU GENERAL PUBLIC LICENSE Version 3
;*
;* Purpose           : Hello World (je, jmp).MASM32
;*
;* Revision History  :
;*
;* Date        Author      Ref    Revision (Date in DDMMYYYY format)
;* 09012019    lst97       1      First release
;*
;* Known Issue       :
;*
;* Note              :
;*
;|**********************************************************************;

;|********************************HEAD***********************************;
.386
.model flat, stdcall ;[cdecl] for c++
option casemap:none ;capitial letter?

include     windows.inc
include     user32.inc
includelib  user32.lib
include     kernel32.inc
includelib  kernel32.lib

;|********************************DATA***********************************;
		.const
szTitle          db	'TITLE (ASSEMBLY)', 0
szContent		     db	'WELCOME TO MASM32!', 0
szContentOK		   db	'You Click OK', 0
szContentCANCEL	 db	'You Click CANCEL', 0

;|********************************CODE***********************************;
		.code
start:
	
		invoke MessageBox, NULL, offset szContent, offset szTitle, MB_OKCANCEL
		cmp eax, IDOK
		je OK
		invoke MessageBox, NULL, offset szContentCANCEL, offset szTitle, MB_OK
		jmp EXIT
		
OK:		invoke MessageBox, NULL, offset szContentOK, offset szTitle, MB_OK
EXIT:	invoke ExitProcess, NULL

;|*******************************END************************************;
		end start
;|**********************************************************************;
