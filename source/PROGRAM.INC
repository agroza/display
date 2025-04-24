; ---------------------------------------------------------------------------
; - DISPLAY - Program Code (program.inc)                                    -
; - Copyright (C) 1998-2025 Alexandru Groza of Microprogramming TECHNIQUES  -
; - All rights reserved.                                                    -
; ---------------------------------------------------------------------------
; - License: GNU General Public License v3.0                                -
; ---------------------------------------------------------------------------

; routines:
;   + programWriteHeader
;   + programMainLoop
;   + programDrawLines
;   + programPreviousLine
;   + programNextLine
; ---------------------------------------------------------------------------

; Writes the program header: blue if file size is below 62K, red otherwise.
; Input:
;     none
; Output:
;     none
; Affects:
;     DX, SI
; Preserves:
;     none
; ---------------------------------------------------------------------------
programWriteHeader proc near

  xor dx, dx
  call videoMoveCursor

  mov byte ptr [GForeground], clWhite
  cmp byte ptr [GFileSize64KLimit], 00h
  je @@blueHeader
  mov byte ptr [GBackground], clRed
  jmp short @@writeText

@@blueHeader:
  mov byte ptr [GBackground], clBlue

@@writeText:
  lea si, sProgram
  call videoWrite

  ret

programWriteHeader endp

; Program main loop.
; Input:
;     none
; Output:
;     CF - set on error, clear otherwise
; Affects:
;     AX, CX, DI, BP, FLAGS
; Preserves:
;     DS
; ---------------------------------------------------------------------------
programMainLoop proc near

  push ds

  mov bp, word ptr [GFileSize]
  mov ax, word ptr [GSegmentAllocated]
  mov ds, ax

  xor di, di

@@updateScreen:
  call videoClearScrollArea
  call programDrawLines

@@readKey:
  xor ah, ah
  int 16h

  cmp ax, kKeyUp
  je @@scrollUp
  cmp ax, kKeyPageUp
  je @@scrollPageUp
  cmp ax, kKeyHome
  je @@scrollHome

  cmp ax, kKeyDown
  je @@scrollDown
  cmp ax, kKeyPageDown
  je @@scrollPageDown
  cmp ax, kKeyEnd
  je @@scrollEnd

  cmp al, kKeyEscape
  je @@exit

  jmp short @@readKey

@@scrollUp:
  or di, di
  jz @@readKey
  call programPreviousLine
  jmp short @@updateScreen

@@scrollPageUp:
  or di, di
  jz @@readKey

  mov cx, kVGARowCount - 1

@@previousLine:
  call programPreviousLine
  loop @@previousLine
  jmp short @@updateScreen

@@scrollHome:
  or di, di
  jz @@readKey
  xor di, di
  jmp short @@updateScreen

@@scrollDown:
  cmp di, bp
  jae @@readKey
  call programNextLine
  jmp short @@updateScreen

@@scrollPageDown:
  cmp di, bp
  jae @@readKey

  mov cx, kVGARowCount - 1

@@NextLine:
  call programNextLine
  loop @@NextLine
  jmp short @@updateScreen

@@scrollEnd:
  cmp di, bp
  jae @@readKey
  mov di, bp
  jmp short @@updateScreen

@@exit:
  pop ds

  ret

programMainLoop endp

; Draws the lines of text.
; Input:
;     none
; Output:
;     none
; Affects:
;     AX, BX, CX, DX, SI, FLAGS
; Preserves:
;     ES, DI, BP
; ---------------------------------------------------------------------------
programDrawLines proc near

  push bp
  push es
  push di

  mov ax, kVGARAMAddress
  mov es, ax

  mov si, di

  mov bx, 1
  mov cx, kVGARowCount - 1

@@lineLoop:
  mov ax, kVGARAMRowWidth
  mul bx
  mov di, ax

  mov bp, kVGAColumnCount
  mov ah, attrNormal

@@charLoop:
  cmp si, word ptr cs:[GFileSize]
  jae @@fillSpaces

  lodsb

  cmp al, kByteCR
  je @@charLoop
  cmp al, kByteLF
  je @@fillSpaces

  stosw

  dec bp
  jnz @@charLoop
  jmp short @@nextLine

@@fillSpaces:
  mov al, kByteSpace

@@fillLoop:
  stosw

  dec bp
  jnz @@fillLoop

@@nextLine:
  inc bx
  loop @@lineLoop

  pop di
  pop es
  pop bp

  ret

programDrawLines endp

; Moves to the start of the next line.
; Input:
;     BP - file size
;     DI - current file offset in RAM
; Output:
;     DI - updated file offset in RAM
; Affects:
;     SI
; Preserves:
;     none
; ---------------------------------------------------------------------------
programNextLine proc near

  mov si, di

@@findLF:
  cmp si, bp
  jae @@exit
  cmp byte ptr [si], kByteLF
  je @@found
  inc si
  cmp si, bp
  jb @@findLF
  mov di, bp
  jmp short @@exit

@@found:
  inc si
  mov di, si

@@exit:
  ret

programNextLine endp

; Moves to the start of the previous line.
; Input:
;     DI - current file offset in RAM
; Output:
;     DI - updated file offset in RAM
; Affects:
;     BX, SI
; Preserves:
;     none
; ---------------------------------------------------------------------------
programPreviousLine proc near

  mov si, di

  mov bx, 02h

@@findLF:
  or si, si
  jnz @@continue
  xor di, di
  jmp short @@exit

@@continue:
  cmp byte ptr [si], kByteLF
  je @@decBX
  dec si
  jmp short @@findLF

@@decBX:
  dec bx
  je @@found
  dec si
  jmp short @@findLF

@@found:
  inc si
  mov di, si

@@exit:
  ret

programPreviousLine endp
