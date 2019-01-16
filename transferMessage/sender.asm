;|**********************************************************************;
;* Project           : Basic_WIN32_ASSEMBLY
;*
;* Program name      : sender.asm
;*
;* Author            : SIO TOU LAI (laisiotou1997@gmail.com)
;*
;* Date created      : 16/01/2019
;*
;* Version           : Internal
;*
;* Copyright         : GNU GENERAL PUBLIC LICENSE Version 3
;*
;* Purpose           : A program that send a message to RECEIVER. MASM32
;*
;* Revision History  :
;*
;* Date        Author      Ref    Revision (Date in DDMMYYYY format)
;* 16012019    lst97       1      First release
;*
;* Known Issue       :
;*
;* Note              :
;*
;|**********************************************************************;

;|********************************HEAD***********************************;
.386
.model flat, stdcall
option casemap:none

include		windows.inc

include		user32.inc
includelib	user32.lib

include		kernel32.inc
includelib	kernel32.lib


;|********************************DATA***********************************;
.data?
	hWnd			dd	?
	szBuffer		dd	256 dup (?)

.const
	szClassName		db	'WIN32 ASSEMBLY - SENDER', 0
	
	szMesBoxAName	db	'WIN32 ASSEMBLY - message', 0
	szMesBoxAText	db	'Press OK to send the message. (Perimeter: %08X)', 0
	szMesBoxATextOK	db	'Message send!', 0
	
	szDestClass		db	'WIN32 ASSEMBLY - RECEIVER', 0
	szText			db	'HELLO WORLD!', 0
	szNotFound		db	'PROGRAM NOT FOUND! (WIN32 ASSEMBLY - RECEIVER)', 0
;|********************************CODE***********************************;
.code
;= = = = = = = = = = = = = = = = Entrance = = = = = = = = = = = = = = = = = 
	main:
		invoke	FindWindow, addr szDestClass, NULL
		.if eax
				mov hWnd, eax
				invoke wsprintf, addr szBuffer, addr szMesBoxAText, addr szText
				invoke MessageBox, NULL, offset szBuffer, offset szClassName, MB_OK
				invoke SendMessage, hWnd, WM_SETTEXT, 0, addr szText
				invoke MessageBox, NULL, offset szMesBoxATextOK, offset szClassName, MB_OK
				
		.else
				invoke MessageBox, NULL, offset szNotFound, offset szMesBoxAName, MB_OK
		.endif
	EXIT:
		invoke ExitProcess, NULL

;|*******************************END*************************************;
end main
;|***********************************************************************;
