; ---------------------------------------------------------------------------
; - VersaVision Video Driver (Adapted) - Assembler Edition (video.inc)      -
; - Integrant part of VersaVision Framework                                 -
; - Copyright (C) 1998-2021 Alexandru Groza of Microprogramming TECHNIQUES  -
; - All rights reserved.                                                    -
; ---------------------------------------------------------------------------
; - License: GNU General Public License v3.0                                -
; ---------------------------------------------------------------------------

; routines:
;   + videoClearScrollArea
;   + videoEnableCursor
;   + videoDisableCursor
;   + videoMoveCursor
;   + videoScrollUp
;   + videoCalculatePosition
;   + videoWriteNewln
;   + videoWrite
;   + videoWriteln
; ---------------------------------------------------------------------------

; general VGA-related
; ---------------------------------------------------------------------------
  kVGAColumnCount        EQU  80
  kVGARowCount           EQU  25
  kVGARAMRowWidth        EQU  160
  kVGARAMAddress         EQU  0B800h

; VGA colors
; ---------------------------------------------------------------------------
  clBlack                EQU  00h
  clBlue                 EQU  01h
  clGreen                EQU  02h
  clCyan                 EQU  03h
  clRed                  EQU  04h
  clMagenta              EQU  05h
  clBrown                EQU  06h
  clLightGray            EQU  07h
  clDarkGray             EQU  08h
  clLightBlue            EQU  09h
  clLightGreen           EQU  0Ah
  clLightCyan            EQU  0Bh
  clLightRed             EQU  0Ch
  clLightMagenta         EQU  0Dh
  clYellow               EQU  0Eh
  clWhite                EQU  0Fh

; VGA color attributes
; ---------------------------------------------------------------------------
  attrNormal             EQU  07h

; ASCII bytes
; ---------------------------------------------------------------------------
  kByteLF                EQU  0Ah
  kByteCR                EQU  0Dh
  kByteSpace             EQU  20h

; global variables section (including preinitialized)
; ---------------------------------------------------------------------------
  GForeground            DB   clLightGray
  GBackground            DB   clBlack

; Clears the 80 x 24 scrollable text area.
; Input:
;     none
; Output:
;     none
; Affects:
;     AX, CX
; Preserves:
;     ES, DI
; ---------------------------------------------------------------------------
videoClearScrollArea proc near

  push es
  push di

  mov ax, kVGARAMAddress
  mov es, ax
  xor di, di
  add di, kVGARAMRowWidth

  mov cx, kVGAColumnCount * (kVGARowCount - 1)
  mov ah, attrNormal
  mov al, kByteSpace

  cld

  rep stosw

  pop di
  pop es

  ret

videoClearScrollArea endp

; Shows the cursor.
; Input:
;     none
; Output:
;     none
; Affects:
;     AH, CX
; Preserves:
;     none
; ---------------------------------------------------------------------------
videoEnableCursor proc near

  mov ah, 01h
  mov cx, 0607h
  int 10h

  ret

videoEnableCursor endp

; Hides the cursor.
; Input:
;     none
; Output:
;     none
; Affects:
;     AH, CH
; Preserves:
;     none
; ---------------------------------------------------------------------------
videoDisableCursor proc near

  mov ah, 01h
  mov ch, 20h
  int 10h

  ret

videoDisableCursor endp

; Moves the cursor to the specified row and column.
; Input:
;     DH - row
;     DL - column
; Output:
;     none
; Affects:
;     AH, BH
; Preserves:
;     none
; ---------------------------------------------------------------------------
videoMoveCursor proc near

  mov ah, 02h
  xor bh, bh
  int 10h

  ret

videoMoveCursor endp

; Scrolls the entire screen contents up.
; Input:
;     none
; Output:
;     none
; Affects:
;     AX, BH, CX, FLAGS
; Preserves:
;     DX
; ---------------------------------------------------------------------------
videoScrollUp proc near

  push dx

  mov ah, 08h
  xor bh, bh
  int 10h
  mov bh, ah

  mov ah, 06h
  mov al, 01h
  xor cx, cx
  mov dh, kVGARowCount - 1
  mov dl, kVGAColumnCount - 1
  int 10h

  pop dx

  ret

videoScrollUp endp

; Calculates the new position in VGA RAM.
; Input:
;     DH - row
;     DL - column
; Output:
;     DI - new offset in VGA RAM
; Affects:
;     BX, CX, FLAGS
; Preserves:
;     AX, DX
; ---------------------------------------------------------------------------
videoCalculatePosition proc near

  push ax
  push dx

  mov cx, dx

  xor ah, ah
  mov al, ch
  xor bh, bh
  mov bl, kVGAColumnCount
  mul bx
  shl ax, 01h
  xor dh, dh
  mov dl, cl
  shl dx, 01h
  add ax, dx

  mov di, ax

  pop dx
  pop ax

  ret

videoCalculatePosition endp

; Advances the cursor to the next line.
; Input:
;     none
; Output:
;     none
; Affects:
;     AX
; Preserves:
;     none
; ---------------------------------------------------------------------------
videoWriteNewln proc near

  mov ax, 0E0Ah
  int 10h
  mov al, kByteCR
  int 10h

  ret

videoWriteNewln endp

; Writes a null-terminated string directly in VGA RAM.
; Deliberately ignores the 80 character row width, so that text could overlap
; to the next visible line.
; Input:
;     SI - pointer to string
; Output:
;     none
; Affects:
;     AX, CX, DX, ES, DI
; Preserves:
;     none
; ---------------------------------------------------------------------------
videoWrite proc near

  mov ah, 03h
  xor bh, bh
  int 10h

  mov ax, kVGARAMAddress
  mov es, ax

  mov ah, byte ptr [GBackground]
  mov cl, 04h
  shl ah, cl
  add ah, byte ptr [GForeground]

  cld

@@nextByte:
  lodsb

  or al, al
  jz @@exit
  cmp al, kByteCR
  jz @@carriageReturn
  cmp al, kByteLF
  jz @@lineFeed

  call videoCalculatePosition

  stosw

  inc dl
  jmp short @@nextByte

@@carriageReturn:
  xor dl, dl
  jmp short @@nextByte

@@lineFeed:
  cmp dh, kVGARowCount - 1
  jz @@scrollUp
  inc dh
  jmp short @@nextByte

@@scrollUp:
  call videoScrollUp
  jmp short @@nextByte

@@exit:
  call videoMoveCursor

  ret

videoWrite endp

; Writes a null-terminated string directly in VGA RAM and goes to next line.
; Input:
;     SI - pointer to string
; Output:
;     none
; Affects:
;     none
; Preserves:
;     none
; ---------------------------------------------------------------------------
videoWriteln proc near

  call videoWrite
  call videoWriteNewln

  ret

videoWriteln endp
