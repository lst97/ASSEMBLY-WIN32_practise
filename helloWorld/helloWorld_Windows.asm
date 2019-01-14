;|**********************************************************************;
;* Project           : Basic_WIN32_ASSEMBLY
;*
;* Program name      : helloWorld_Windows.asm
;*
;* Author            : SIO TOU LAI (laisiotou1997@gmail.com)
;*
;* Date created      : 14/01/2019
;*
;* Version           : Internal
;*
;* Copyright         : GNU GENERAL PUBLIC LICENSE Version 3
;*
;* Purpose           : Full Windows Hello World.MASM32
;*
;* Revision History  :
;*
;* Date        Author      Ref    Revision (Date in DDMMYYYY format)
;* 14012019    lst97       1      First release
;*
;* Known Issue       :
;*
;* Note              :
;* code from https://www.bilibili.com/video/av28141241/?p=27
;*
;|**********************************************************************;

;|********************************HEAD***********************************;
.386
.model flat, stdcall ;[cdecl] for c++
option casemap:none ;capitial letter?

include		windows.inc
include		gdi32.inc
includelib	gdi32.lib
include		user32.inc
includelib	user32.lib
include		kernel32.inc
includelib	kernel32.lib

;|********************************DATA***********************************;
.data?
	hInstance		dd	?
	hWinMain		dd	?

.const
	szClassName		db	'WIN32 ASSEMBLY', 0
	szCaptionMain	db	'Hello World_ASSEMBLY', 0
	szText			db	'Welcome to WIN32_ASM!', 0
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
				invoke	DestroyWindow, hWinMain
				invoke	PostQuitMessage, NULL
				
		.else
				invoke	DefWindowProc, hWnd, uMsg, wParam, lParam
				ret
		.endif
		
		xor		eax, eax
		ret
			
	_ProcWinMain	endp
	
	_WinMain		proc
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
				WS_OVERLAPPEDWINDOW, 100, 100, 600, 400,\
				NULL, NULL, hInstance, NULL
		mov		hWinMain, eax
		invoke	ShowWindow, hWinMain, SW_SHOWNORMAL
		invoke	UpdateWindow, hWinMain

;= = = = = = = = = = = = = = = Message Loop = = = = = = = = = = = = = = = = 
		.while	TRUE
				invoke	GetMessage, addr @stMsg, NULL, 0, 0
				.break	.if	eax == 0
				invoke	TranslateMessage, addr @stMsg
				invoke	DispatchMessage, addr @stMsg
		.endw
		ret
		
	_WinMain endp

;= = = = = = = = = = = = = = = = Entrance = = = = = = = = = = = = = = = = = 
	main:
		call _WinMain
	EXIT:
		invoke ExitProcess, NULL

;|*******************************END*************************************;
end main
;|***********************************************************************;
