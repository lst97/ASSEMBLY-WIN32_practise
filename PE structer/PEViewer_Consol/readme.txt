/*
;|**********************************************************************;
;* Project           : ASSEMBLY-WIN32_practise (PE_Structure)
;*
;* Program name      : PEinfo.c, FileToImage.c, Source.c, PEInject.c
;*
;* Author            : SIO TOU LAI (laisiotou1997@gmail.com)
;*
;* Date created      : 11/09/2019
;*
;* Version           : internal
;*
;* Copyright         : GNU GENERAL PUBLIC LICENSE Version 3
;*
;* Purpose           : Get PE info from file only support 32bit compiled written MASM
;*
;* Revision History  :
;* Date        Author      Ref    Revision (Date in DDMMYYYY format)
;* 11092019    lst97       1      First release
;* 13092019    lst97       2      Add PE inject function
;* 14092019    lst97       3      Add add sectionTable function
;*
;* Known Issue       :
;* - PEinfo not yet finish, can only read until section table.
;* - Display problem when print sectionName
;* - Only works with MASM compiled program (PE injector)
;* TODO              :
;* + Full PEinfo tool.
;* + PE vaildate (to see if it's a valid PE file)
;* + Translation.
;* & Any suggestion?
;*
;|**********************************************************************;
*/
