; ---------------------------------------------------------------------------
; - DISPLAY - Routines (routines.inc)                                       -
; - Copyright (C) 1998-2025 Alexandru Groza of Microprogramming TECHNIQUES  -
; - All rights reserved.                                                    -
; ---------------------------------------------------------------------------
; - License: GNU General Public License v3.0                                -
; ---------------------------------------------------------------------------

; routines:
;   + cmdLineCheckDelimiter
;   + cmdLineGetFilename
;   + fileCheckLimitSize
;   + fileOpenLoadClose
; ---------------------------------------------------------------------------

; Checks whether the given character is a commandline delimiter.
; Input:
;     AL - character to test
; Output:
;     ZF - set if delimiter, clear otherwise
; Affects:
;     FLAGS
; Preserves:
;     none
; ---------------------------------------------------------------------------
cmdLineCheckDelimiter proc near

  cmp al, ' '
  jz @@exit
  cmp al, ','
  jz @@exit
  cmp al, 09h
  jz @@exit
  cmp al, ';'
  jz @@exit
  cmp al, '='
  jz @@exit
  cmp al, 0Dh

@@exit:
  ret

cmdLineCheckDelimiter endp

; Parses the commandline for a filename, skipping delimiters.
; Input:
;     DS:SI - commandline segment (SI = 80h)
; Output:
;     SI - pointer to filename start
;     DX - same as SI
;     CF - set on error (no filename)
; Affects:
;     AL, CX, SI, FLAGS
; Preserves:
;     none
; ---------------------------------------------------------------------------
cmdLineGetFilename proc near

  cld

  mov si, 80h
  xor ch, ch
  mov cl, byte ptr ds:[si]
  inc si
  add si, cx
  mov byte ptr ds:[si], 00h
  mov si, 81h

@@nextByte:
  lodsb

  cmp al, kByteCR
  jz @@checkFilename

  call cmdLineCheckDelimiter
  jz @@nextByte

@@checkFilename:
  dec si
  cmp byte ptr ds:[si], 00h
  jz @@setError
  jmp short @@exit

@@setError:
  stc

@@exit:
  mov dx, si

  ret

cmdLineGetFilename endp

; Checks whether the file size exceeds 62K. If yes, caps it to 62K.
; Input:
;     DX:AX - 32-bit file size from INT 21h / AL = 42h
; Output:
;     AX - size (caped or original)
; Affects:
;     FLAGS
; Preserves:
;     none
; ---------------------------------------------------------------------------
fileCheckLimitSize proc near

  mov word ptr [GFileSize], ax

  or dx, dx
  jnz @@capSize
  cmp ax, kMaxFileSize
  ja @@capSize
  jmp short @@exit

@@capSize:
  mov ax, kMaxFileSize
  mov word ptr [GFileSize], ax
  inc byte ptr [GFileSize64KLimit]

@@exit:
  ret

fileCheckLimitSize endp

; Opens a file and loads it into RAM buffer, then closes it.
; Input:
;     DS:SI - commandline segment (SI = 80h)
; Output:
;     DS:[GSegmentAllocated] - memory segment where the file is loaded
;     AX - last DOS service return (if needed)
;     SI - points to error message (if error occured)
;     CF - clear on success, set on failure
; Affects:
;     AX, BX, CX, DX, DS, ES, SI, DI, FLAGS
; Preserves:
;     none
; ---------------------------------------------------------------------------
fileOpenLoadClose proc near

  call cmdLineGetFilename
  jc @@parameterError

  mov ah, 3Dh
  xor al, al
  int 21h
  jc @@fileError
  mov bx, ax

  mov ah, 42h
  mov al, 02h
  xor cx, cx
  xor dx, dx
  int 21h
  jc @@fileError
  mov word ptr [GFileHandle], bx

  call fileCheckLimitSize

  mov ah, 42h
  xor al, al
  xor cx, cx
  xor dx, dx
  int 21h
  jc @@fileError

  mov ah, 4Ah
  mov bx, offset programEnd
  add bx, 0Fh
  mov cl, 04h
  shr bx, cl
  int 21h
  jc @@memoryError

  mov ah, 48h
  mov bx, word ptr [GFileSize]
  add bx, 0Fh
  mov cl, 04h
  shr bx, cl
  int 21h
  jc @@memoryError
  mov word ptr [GSegmentAllocated], ax

  push ds

  mov ds, ax
  mov es, ax

  xor di, di

  mov ah, 3Fh
  mov bx, word ptr cs:[GFileHandle]
  mov cx, word ptr cs:[GFileSize]
  xor dx, dx
  int 21h

  pop ds

  jc @@fileError

  mov ah, 3Eh
  int 21h
  jc @@fileError

  jmp short @@exit

@@parameterError:
  lea si, sParameterError
  jmp short @@exit

@@fileError:
  lea si, sFileError
  jmp short @@exit

@@memoryError:
  lea si, sMemoryError

@@exit:
  ret

fileOpenLoadClose endp
