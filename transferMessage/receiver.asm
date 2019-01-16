;|**********************************************************************;
;* Project           : Basic_WIN32_ASSEMBLY
;*
;* Program name      : receiver.asm
;*
;* Author            : SIO TOU LAI (laisiotou1997@gmail.com)
;*
;* Date created      : 16/01/2019
;*
;* Version           : Internal
;*
;* Copyright         : GNU GENERAL PUBLIC LICENSE Version 3
;*
;* Purpose           : Receive a message. MASM32
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

include		shell32.inc
includelib	shell32.lib

;|********************************DATA***********************************;
.data?
	hInstance		dd	?
	hWinMain		dd	?
	szBuffer		db	512 dup (?)
  
.const
	szClassName		db	'WIN32 ASSEMBLY - RECEIVER', 0
	szCaptionMain	db	'WIN32 ASSEMBLY - RECEIVER', 0
	szText			db	'Welcome to WIN32_ASM!', 0
	
	szMesBoxAText	db	'RECEIVER is running! EXIT...', 0
	
	szReceive		db	'WM_SETTEXT event detected! (Param: %08X)', 0Dh, 0Ah
					db	'String is: "%s"', 0
					
	szMesBoxBText	db	'Use KEY to close!', 0
;|********************************CODE***********************************;
.code
;= = = = = = = = = = = = = = Windows Process = = = = = = = = = = = = = = = 
	_ProcWinMain	proc	uses	ebx	edi	esi	hWnd, uMsg, wParam, lParam
		LOCAL	@stPs:PAINTSTRUCT
		LOCAL	@stRect:RECT
		LOCAL	@hDc
		
		mov		eax, uMsg
		
		.if		eax == WM_PAINT
				invoke	BeginPaint, hWnd, addr @stPs
				mov 	@hDc, eax
				invoke	GetClientRect, hWnd, addr @stRect
				invoke 	DrawText, @hDc, addr szText, -1,\
						addr @stRect, DT_SINGLELINE or DT_CENTER or DT_VCENTER
				invoke	EndPaint, hWnd, addr @stPs
		.elseif	eax == WM_CLOSE
				invoke	MessageBox, hWnd, offset szMesBoxBText, addr szCaptionMain, MB_OK
				
		.elseif	eax == WM_SETTEXT
				invoke	wsprintf, addr szBuffer, addr szReceive, lParam, lParam
				invoke	MessageBox, hWnd, offset szBuffer, addr szCaptionMain, MB_OK
				
				invoke	DestroyWindow, hWinMain
				invoke	PostQuitMessage, NULL
		.else
				invoke	DefWindowProc, hWnd, uMsg, wParam, lParam		;Handle other events
				ret
		.endif
		
		xor		eax, eax
		ret
			
	_ProcWinMain	endp
	
	_WinMain	proc
		LOCAL	@stWndClass:WNDCLASSEX
		LOCAL	@stMsg:MSG
		
		invoke	GetModuleHandle, NULL
		mov		hInstance, eax
		invoke	RtlZeroMemory, addr @stWndClass, sizeof @stWndClass
		
;= = = = = = = = = = = = = = Register Windows = = = = = = = = = = = = = = = 
		invoke	LoadCursor, 0, IDC_ARROW
		mov		@stWndClass.hCursor, eax
		push	hInstance
		pop		@stWndClass.hInstance
		mov		@stWndClass.cbSize, sizeof WNDCLASSEX
		mov		@stWndClass.style, CS_HREDRAW or CS_VREDRAW
		mov		@stWndClass.lpfnWndProc, offset _ProcWinMain
		mov		@stWndClass.hbrBackground, COLOR_WINDOW +1
		mov 	@stWndClass.lpszClassName, offset szClassName
		invoke	RegisterClassEx, addr @stWndClass

;= = = = = = = = = = = = = = Build & Show Win = = = = = = = = = = = = = = = 
		invoke	CreateWindowEx, WS_EX_CLIENTEDGE, offset szClassName, offset szCaptionMain,\
				WS_OVERLAPPEDWINDOW, 128, 128, 600, 400,\
				NULL, NULL, hInstance, NULL
		mov		hWinMain, eax
		invoke	ShowWindow, hWinMain, SW_SHOWNORMAL
		invoke	UpdateWindow, hWinMain

;= = = = = = = = = = = = = = = Message Loop = = = = = = = = = = = = = = = = 
		.while	TRUE
				invoke	GetMessage, addr @stMsg, NULL, 0, 0
				.break .if eax == 0
				;invoke	TranslateMessage, addr @stMsg
				invoke 	DispatchMessage, addr @stMsg		;GetMessage than to _ProcWinMain
		.endw
		ret
		
	_WinMain endp

;= = = = = = = = = = = = = = = = Entrance = = = = = = = = = = = = = = = = = 
	main:
		invoke	FindWindow, addr szClassName, NULL
		.if eax
				invoke MessageBox, NULL, offset szMesBoxAText, offset szCaptionMain, MB_OK
				jmp EXIT
		.endif
		call _WinMain
	EXIT:
		invoke ExitProcess, NULL

;|*******************************END*************************************;
end main
;|***********************************************************************;
